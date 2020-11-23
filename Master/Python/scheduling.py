#!/usr/local/bin/python3
from scheduling_configuration import Scheduling_algorithm, Scheduling_strategies, Scheduling_window_size_algorithm
from flow import Flow
from cell import Cell
from cell_c_llf import Cell_C_LLF

class Schedule(object):

  def __init__(self, flows): # , graph
    # self.graph = graph  # Dict{sender:Dict{receiver:etx}}
    self.flows = flows  # List[Flow]


  def order_flows_decreasing(self):
    self.flows.sort(key=lambda x: x.length, reverse=True)

  # "more abstracted" sort method
  def order_by_(self, object_to_sort, criterion):
    object_to_sort.sort(key = criterion)

  def available_channel_in_slot(self, slot_index):
    for channel_idx in range(len(self.schedule)):
      if self.schedule[channel_idx][slot_index] == 0:
        return channel_idx
    return False

  def schedulable_in_slot(self, slot_index, participants):
    slot_available = False
    for channel in self.schedule:
      if channel[slot_index] == 0:
        slot_available = True
    if not slot_available:
      return False
    if len(self.used_nodes_in_timeslot[slot_index]) == 0: # no participant in slot
      return True
    for participant in participants:
      if participant in self.used_nodes_in_timeslot[slot_index]:
        return False
    return True

  # finds the next free pair of channel and timeslot
  def find_schedulable_slot(self, cell, release_time_idx):
    for timeslot_idx in range(len(self.schedule[0])):
      for channel_idx in range(len(self.schedule)):
        if isinstance(self.schedule[channel_idx][timeslot_idx], Cell):
          # are there collisions with other nodes?
          if not set(cell.participants).intersection(set(self.schedule[channel_idx][timeslot_idx].participants)) == set():
            break
        # considers the release time if not zero
        if self.schedule[channel_idx][timeslot_idx] == 0 and timeslot_idx >= release_time_idx:
          return (channel_idx, timeslot_idx)
    # flow could not be scheduled
    return None

  # computes the laxity for a given flow and timeslot
  def compute_laxity(self, cell_c_llf, to_release, timeslot_idx):
    # finds all flows which release time is at least the current release time and not later as the deadline of the current to schedule flow (implementation of equation 5)
    intersection_flows = list(filter(lambda x: cell_c_llf.flow.release_time <= x.flow.release_time <= cell_c_llf.flow.deadline and not set(x.cell.participants).intersection(set([cell_c_llf.flow.source])) == set(), to_release))

    critical_window = []
    # calculates the laxity (implementation of equation 6)
    for flow in intersection_flows:
      critical_window.append(((flow.flow.deadline - timeslot_idx + 1) - len(list(filter(lambda x: timeslot_idx <= x.flow.release_time and x.flow.deadline <= flow.flow.deadline, intersection_flows)))))
    # finds the smallest laxity if possible (implementation of equation 7)
    if not critical_window == []:
      cell_c_llf.laxity = min(critical_window)

  def create(self, etx_power, num_channels, shortest_repeating_cycle, algorithm, ignore_NSF, strategy, window_size_algorithm=None, fixed_window_size=None):
    self.num_channels = num_channels
    self.strategy = strategy

    max_schedule_length = 0
    max_time_slot = 0
    # initialization of each flow
    for flow in self.flows:
      flow.compute_path(etx_power)
    #   flow.modify_path(strategy, window_size)
      max_schedule_length += flow.length
      flow.create_cells(strategy, window_size_algorithm, fixed_window_size)
      flow.set_CNMID(False)
      if max_time_slot < flow.release_time:
        max_time_slot = flow.release_time
      # adjusts the release time accordingly to the hyper-period
      if (shortest_repeating_cycle > 1 and flow.release_time > shortest_repeating_cycle) or (not shortest_repeating_cycle == 1 and flow.release_time >= shortest_repeating_cycle):
        flow.release_time %= shortest_repeating_cycle

    # init schedule
    self.schedule = [[0] * (max_schedule_length * shortest_repeating_cycle + max_time_slot) for _ in range(num_channels)]
    self.used_nodes_in_timeslot = [[] for _ in range(len(self.schedule[0]))]

    # to store all infeasible flows (relevant for ADVISE and ADJUST mode) [part of the safty module]
    infeasible_flows = []

    if algorithm == Scheduling_algorithm.R_LPF:
      ignore_schedulability = False
      if not list(filter(lambda x: x.deadline == None, self.flows)) == []:
        ignore_schedulability = True
      last_timeslot = 0
      self.order_flows_decreasing()
      insert_at_index = 0
      current_flow_number = -1
      current_idx = 0
      for flow in self.flows:
        # last_index = -1
        insert_at_index = 0
        if current_flow_number == -1 or not current_flow_number == flow.flow_number:
          current_flow_number = flow.flow_number
          current_idx = 0
        for cell in reversed(flow.cells):
          while not self.schedulable_in_slot(insert_at_index, cell.participants) or insert_at_index < current_idx:
            insert_at_index += 1
          channel = self.available_channel_in_slot(insert_at_index)
          # schedulability analysis
          if not ignore_schedulability and (insert_at_index >= flow.release_time + flow.deadline or (not shortest_repeating_cycle == 1 and insert_at_index + 1 > shortest_repeating_cycle)):
            # flow misses its deadline
            flow.set_CNMID(True)
            # INFEASIBLE mode (see design chapter)
            if ignore_NSF == "infeasible":
              raise Exception("error@scheduling.py: infeasible scheduler!")
          # ADVISE and ADJUST mode (see design chapter)
          if ignore_schedulability or (ignore_NSF == "disabled" or ignore_NSF == "infeasible" or (ignore_NSF == "enabled" and not flow.CNMID)):
            self.schedule[channel][insert_at_index] = cell
            self.used_nodes_in_timeslot[insert_at_index].extend(cell.participants)
            if insert_at_index > last_timeslot:
              last_timeslot = insert_at_index
          else:
            infeasible_flows.append((flow, cell.participants))
        current_idx += flow.period

    # ensures that deadlines are available for using EDF, RMS, or C-LLF [part of the safty module]
    if not (algorithm == Scheduling_algorithm.R_LPF or list(filter(lambda x: x.deadline == None, self.flows)) == []):
      raise Exception("error@scheduling.py: for scheduling with EDF, RMS, or C-LLF, deadlines are needed!")

    # EDF implementation:
    if algorithm == Scheduling_algorithm.EDF:
      last_timeslot = 0
      # sorts the list of flows accordingly to the deadline
      self.order_by_(self.flows, lambda x: x.deadline)
      for flow in self.flows:
        release_time = flow.release_time
        for cell in flow.cells:
          # next free pair of channel and timeslot
          schedule_idx = self.find_schedulable_slot(cell, release_time)
          # is there a free pair left?
          if schedule_idx:
            # schedulability analysis
            if schedule_idx[1] >= flow.release_time + flow.deadline or (not shortest_repeating_cycle == 1 and schedule_idx[1] + 1 > shortest_repeating_cycle):
              # flow misses its deadline
              flow.set_CNMID(True)
              # INFEASIBLE mode (see design chapter)
              if ignore_NSF == "infeasible":
                raise Exception("error@scheduling.py: infeasible scheduler!")
            # ADVISE and ADJUST mode (see design chapter)
            if ignore_NSF == "disabled" or ignore_NSF == "infeasible" or (ignore_NSF == "enabled" and not flow.CNMID):
              self.schedule[schedule_idx[0]][schedule_idx[1]] = cell
              release_time = schedule_idx[1] + 1
              if schedule_idx[1] > last_timeslot:
                last_timeslot = schedule_idx[1]
            else:
              infeasible_flows.append((flow, cell.participants))
          else:
            raise ValueError("error@scheduling.py: flow {" + "{:2d} -> {:2d}".format(flows.source, flow.destination) + "} is faulty!")

    # RMS implementation:
    if algorithm == Scheduling_algorithm.RMS:
      last_timeslot = 0
      # sorts the list of flows accordingly to the period
      self.flows = sorted(self.flows, key = lambda x: (x.period, x.deadline))
      for flow in self.flows:
        release_time = flow.release_time
        for cell in flow.cells:
          # next free pair of channel and timeslot
          schedule_idx = self.find_schedulable_slot(cell, release_time)
          # is there a free pair left?
          if schedule_idx:
            # schedulability analysis
            if schedule_idx[1] >= flow.release_time + flow.deadline or (not shortest_repeating_cycle == 1 and schedule_idx[1] + 1 > shortest_repeating_cycle):
              # flow misses its deadline
              flow.set_CNMID(True)
              # INFEASIBLE mode (see design chapter)
              if ignore_NSF == "infeasible":
                raise Exception("error@scheduling.py: infeasible scheduler!")
            # ADVISE and ADJUST mode (see design chapter)
            if ignore_NSF == "disabled" or ignore_NSF == "infeasible" or (ignore_NSF == "enabled" and not flow.CNMID):
              self.schedule[schedule_idx[0]][schedule_idx[1]] = cell
              release_time = schedule_idx[1] + 1
              if schedule_idx[1] > last_timeslot:
                last_timeslot = schedule_idx[1]
            else:
              infeasible_flows.append((flow, cell.participants))
          else:
            raise ValueError("error@scheduling.py: flow {" + "{:2d} -> {:2d}".format(flows.source, flow.destination) + "} is faulty!")

    # C-LLF implementation:
    if algorithm == Scheduling_algorithm.C_LLF:
      last_timeslot = 0
      transmissions_flows = []
      # initialization of C-LLF (preprocessing of each flow/lifting each flow to the Cell_C-LLF data structure)
      for flow in self.flows:
        for cell in flow.cells:
          transmissions_flows.append(Cell_C_LLF(flow, cell))
      timeslot_idx = 0
      while transmissions_flows:
        # finds all flows which are now to release
        to_release = list(filter(lambda x: x.flow.release_time <= timeslot_idx, transmissions_flows))
        # computes the laxity for all to release flows
        for transmissions_flow in transmissions_flows:
          self.compute_laxity(transmissions_flow, to_release, timeslot_idx)
        channel_idx = 0
        while channel_idx < len(self.schedule) and to_release:
          # sorts the list of flows accordingly to the laxity
          self.order_by_(to_release, lambda x: x.laxity)
          # only the smallest laxities are relevant for this round
          transmissions = list(filter(lambda x: x.laxity == to_release[0].laxity, to_release))
          # sorts the list of flows accordingly to the deadline (in case of a tie)
          self.order_by_(transmissions, lambda x: x.flow.deadline)
          release_time = transmissions[0].flow.release_time
          for cell in transmissions[0].flow.cells:
            # next free pair of channel and timeslot
            schedule_idx = self.find_schedulable_slot(cell, release_time)
            # is there a free pair left?
            if schedule_idx:
              # schedulability analysis
              if schedule_idx[1] >= transmissions[0].flow.release_time + transmissions[0].flow.deadline or (not shortest_repeating_cycle == 1 and schedule_idx[1] + 1 > shortest_repeating_cycle):
                # flow misses its deadline
                transmissions[0].flow.set_CNMID(True)
                # INFEASIBLE mode (see design chapter)
                if ignore_NSF == "infeasible":
                  raise Exception("error@scheduling.py: infeasible scheduler!")
              # ADVISE and ADJUST mode (see design chapter)
              if ignore_NSF == "disabled" or ignore_NSF == "infeasible" or (ignore_NSF == "enabled" and not transmissions[0].flow.CNMID):
                self.schedule[schedule_idx[0]][schedule_idx[1]] = cell
                release_time = schedule_idx[1] + 1
                transmissions_flows = list(filter(lambda x: not (cell == x.cell and transmissions[0].flow.sub_flow_number == x.flow.sub_flow_number), transmissions_flows))
              else:
                infeasible_flows.append((transmissions[0].flow, transmissions[0].cell.participants))
          # removes every conflicting transmission
          participants = []
          for cell in transmissions[0].flow.cells:
            participants.append(cell.participants)
          to_release = list(filter(lambda x: set(x.cell.participants).intersection() == set(participants), to_release))
          channel_idx += 1
        timeslot_idx += 1

    # adjusts the schedule's overall size
    if shortest_repeating_cycle > 1:
      last_timeslot = shortest_repeating_cycle - 1

    # remove unused cells at end
    for channel_idx, channel in enumerate(self.schedule):
      self.schedule[channel_idx] = channel[:last_timeslot+1]
      for timeslot in range(len(self.schedule[channel_idx])//2):
        if not (algorithm == Scheduling_algorithm.EDF or algorithm == Scheduling_algorithm.RMS or algorithm == Scheduling_algorithm.C_LLF):
          tmp_timeslot_cell = self.schedule[channel_idx][timeslot]
          self.schedule[channel_idx][timeslot] = self.schedule[channel_idx][last_timeslot-timeslot]
          self.schedule[channel_idx][last_timeslot-timeslot] = tmp_timeslot_cell
      # set cell attributes
      for timeslot in range(last_timeslot+1):
        cell = self.schedule[channel_idx][timeslot]
        if isinstance(cell, Cell):
         cell.set_schedule_attributes(timeslot, channel_idx)

    # adjusts the flow sets if flows are infeasible (relevant for ADVISE and ADJUST mode) [part of the safty module]
    if infeasible_flows:
      print("information@scheduling.py: the following" + ("", "s") [len(infeasible_flows) > 1] + " {" + " | ".join("({:2d} -> {:2d})".format(str(infeasible_flow[1][0]), str(infeasible_flow[1][1])) for infeasible_flow in infeasible_flows) + "} " + "can't meet " + ("its", "their") [len(infeasible_flows) > 1] + " deadline.\n")
      # removes infeasible flows
      while infeasible_flows:
        self.flows = list(filter(lambda x: not x.flow_number == infeasible_flows[0][0].flow_number, self.flows))
        # updates the flow number
        for flow in self.flows:
          if infeasible_flows[0][0].flow_number < flow.flow_number:
            flow.flow_number -= 1
            for cell in flow.cells:
              cell.flow_number = flow.flow_number
        infeasible_flows.remove(infeasible_flows[0])
        for infeasible_flow in infeasible_flows:
          if infeasible_flows[0][0].flow_number < infeasible_flow[0].flow_number:
            infeasible_flow[0].flow_number -= 1

  def get_node_schedule(self, *nodes):
    # returns schedule for each node
    node_schedule = {}
    for timeslot in range(len(self.schedule[0])):
      for channel in range(len(self.schedule)):
        cell = self.schedule[channel][timeslot]
        if isinstance(cell, Cell):
          for participant in cell.participants:
            if participant in nodes:
              if participant not in node_schedule:
                node_schedule[participant] = []
              node_schedule[participant].append(cell)
    return node_schedule

  def get_max_cell_participants(self):
    max_num_cell_participants = 0
    for timeslot in range(len(self.schedule[0])):
      for channel in range(len(self.schedule)):
        cell = self.schedule[channel][timeslot]
        if isinstance(cell, Cell):
          num_participants = len(cell.participants)
          if num_participants > max_num_cell_participants:
            max_num_cell_participants = num_participants
    return max_num_cell_participants

  def get_flow_from_id(self, id):
    for flow in self.flows:
      if id == flow.flow_number:
        return flow
    return False

  def __str__(self):
    string = "Scheduling strategy: {}\n"
    separator = " | "
    # cell_character_width = len("nn -> nn (nn -> nn)")
    cell_character_width = 19
    if self.strategy == Scheduling_strategies.no_retransmissions:
      string = string.format("no retransmissions")
    elif self.strategy == Scheduling_strategies.upper_etx_tx_per_link:
      string = string.format("slot-based upper etx")
    elif self.strategy == Scheduling_strategies.sliding_window:
      string = string.format("sliding windows")
      max_num_cell_participants = self.get_max_cell_participants()
      # cell_character_width = len("nn, ") * max_num_cell_participants - len(",") + len("(nn -> nn)")
      cell_character_width = 4 * max_num_cell_participants - 1 + 10
      empty_participant = "    " # "nn, "
    else:
      string.format("unknown")

    # line_separator = (len("nnn | ") + (cell_character_width + len(" | ")) * self.num_channels - len(" ")) * "-" + "\n"
    line_separator = (10 + (cell_character_width + 10) * self.num_channels - 1) * "-" + "\n"
    empty = " " * cell_character_width

    for timeslot in range(len(self.schedule[0])):
      string += line_separator
      string += str(timeslot).rjust(3) + separator
      for channel in range(len(self.schedule)):
        cell = self.schedule[channel][timeslot]
        if not isinstance(cell, Cell):
          string += empty
          string += (" " * 8) + separator
        else:
          if self.strategy == Scheduling_strategies.sliding_window:
            string += ', '.join(str(participant).rjust(2) for participant in cell.participants)
            string += ' '
            num_participants = len(cell.participants)
            string += empty_participant * (max_num_cell_participants - num_participants)
          else:
            sender = cell.participants[0]
            receiver = cell.participants[-1]
            string += "{} -> {} ".format(str(sender).rjust(2), str(receiver).rjust(2))
          flow = self.get_flow_from_id(cell.flow_number)
          string += "({} -> {}) {}".format(str(flow.source).rjust(2), str(flow.destination).rjust(2), ("", "[CNMID]") [flow.CNMID])
          string += (" " * (0, 7) [not flow.CNMID]) + separator
      string += "\n"
    string += line_separator
    return string



