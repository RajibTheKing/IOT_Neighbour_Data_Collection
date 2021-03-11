#!/usr/local/bin/python3

from dataclasses import dataclass
from enum import Enum
from os.path import dirname, abspath
import copy
import dijkstra
from scheduling import Schedule
from scheduling_configuration import Scheduling_strategies
from cell import Cell
from flow import Flow

class Link_options(Enum):
  transmit = 1
  receive = 2

@dataclass
class Link:
  timeslot: int
  channel: int
  link_option: int
  flow_number: int
  neighbor: int # sender or receiver of link


class Contiki_schedule(object):

  def __init__(self, graph, schedule, node_ids, network_time_source=1):
    self.graph = graph
    self.schedule = schedule
    self.node_ids = node_ids
    self.network_time_source = network_time_source

  def get_timesource_for_nodes(self):
    self.timesource = {}
    remaining_nodes = []
    participating_nodes = []
    if hasattr(self, 'node_schedule'):
      timesource_graph = {}
      participating_nodes = [*self.node_schedule]
      if self.network_time_source in participating_nodes:
        for sender in participating_nodes:
          if sender in self.graph:
            for receiver in participating_nodes:
              if receiver in self.graph[sender]:
                if sender not in timesource_graph:
                  timesource_graph[sender] = {}
                timesource_graph[sender][receiver] = self.graph[sender][receiver] ** 2
        for node in participating_nodes:
          if node != self.network_time_source:
            try:
              _, path = dijkstra.shortestPath(timesource_graph, node, self.network_time_source)
              self.timesource[node] = path[1]
            except:
              remaining_nodes.append(node)
      else:
        remaining_nodes = self.node_ids
    else:
      remaining_nodes = self.node_ids

    participating_nodes = set(participating_nodes) - set(remaining_nodes)
    remaining_nodes += (set(self.node_ids) - participating_nodes)
    remaining_nodes.sort()

    timesource_graph = copy.deepcopy(self.graph)
    for sender, receivers in timesource_graph.items():
      for receiver in [*receivers]:
        weight = timesource_graph[sender][receiver]
        timesource_graph[sender][receiver] = weight ** 2

    for node in remaining_nodes:
      if node != self.network_time_source:
        _, path = dijkstra.shortestPath(timesource_graph, node, self.network_time_source)
        self.timesource[node] = path[1]

  def get_flow_from_sender(self, node_id):
    for flow in self.schedule.flows:
      if flow.source == node_id:
        return flow
    return False

  def sorted_list_of_links(self, node_id):
    links = []
    for cell in self.node_schedule[node_id]:
      cell_node_index = cell.participants.index(node_id)

      if cell_node_index == 0:
        link_option = Link_options.transmit.value
        neighbor = cell.participants[1]
      elif cell_node_index == len(cell.participants) -1:
        link_option = Link_options.receive.value
        neighbor = cell.participants[-2]
      else:
        link_option = Link_options.transmit.value | Link_options.receive.value
        neighbor = cell.participants[cell_node_index + 1]

      links.append(Link(cell.timeslot, cell.channel, link_option, cell.flow_number, neighbor))

    links.sort(key=lambda x: (x.neighbor, x.timeslot))
    return links

  def generate(self, output_file_path, flows, minimal_schedule_length=0, with_timesource=False):

    # prepared strings
    str_func                         = 'void generated_schedule(void){\n'
    str_schedule_length               = 'schedule_length = {};\n'

    str_create_slotframe              = 'sf[{0}] = tsch_schedule_get_slotframe_by_handle({0});\n'
    str_create_slotframe             += 'if (sf[{0}]){{\n'
    str_create_slotframe             += '  tsch_schedule_remove_slotframe(sf[{0}]);\n'
    str_create_slotframe             += '}}\n'
    str_create_slotframe             += 'sf[{0}] = tsch_schedule_add_slotframe({0}, schedule_length);\n'

    str_last_received_packet_of_flow  = 'hash_map_insert(&last_received_relayed_packet_of_flow, {}, 0);\n'

    str_ttl_if                        = '#if TSCH_TTL_BASED_RETRANSMISSIONS\n'
    str_ttl_endif                     = '#endif /* TSCH_TTL_BASED_RETRANSMISSIONS */\n'
    str_not_ttl_if                    = '  #if !TSCH_TTL_BASED_RETRANSMISSIONS\n'
    str_not_ttl_endif                 = '  #endif /* !TSCH_TTL_BASED_RETRANSMISSIONS */\n'

    str_first_tx_slot_of_sf           = '  first_tx_slot_in_flow[{}][{}] = {};\n'
    str_last_tx_slot_of_sf            = '  last_tx_slot_in_flow[{}][{}] = {};\n'

    str_start_first_node              = 'if (node_id == {}){{\n'
    str_start_next_node               = '}} else if (node_id == {}){{\n'
    str_end_last_node                 = '}\n'

    str_own_tx_flow                   = '  own_transmission_flow = {};\n'
    str_own_receiver                  = '  is_sender = 1;\n' + \
                                        '  own_receiver = {};\n'

    str_flow_sender                   = 'sender_of_flow[{}] = {};\n'
    str_flow_receiver                 = 'receiver_of_flow[{}] = {};\n'

    str_start_links                   = '  const scheduled_link_t add_link[] = {\n'
    str_link                          = '    {{ {}, {}, {}, {} }},\n'                       # slotframe/flow_number, link_option, timeslot, channel offset
    str_end_links                     = '  };\n'

    str_forward_to                    = '  hash_map_insert(&forward_to, {}, {});\n'           # final receiver, next receiver

    str_change_on_index               = '  const uint8_t cha_idx[] = {{{}}};\n'
    str_change_on_index_to            = '  const uint8_t cha_idx_to[] = {{{}}};\n'

    str_links_call                    = '  add_links(add_link, {}, cha_idx, cha_idx_to, {});\n'

    str_beacon_link                   = 'tsch_schedule_add_link(sf[1], LINK_OPTION_RX | LINK_OPTION_TX | LINK_OPTION_SHARED | LINK_OPTION_TIME_KEEPING, LINK_TYPE_ADVERTISING, &tsch_broadcast_address, {}, 0);'

    str_max_transmissions             = '  max_transmissions[{}] = {};\n'

    str_sending_slots                 = '    sending_slots[{}] = {};\n'                      # index, slotnumber
    str_num_sending_slots             = '    num_sending_slots = {};\n'                      # number of indices above

    if with_timesource:
      str_set_timesource              = '  destination.u8[NODE_ID_INDEX] = {};\n' + \
                                        '  tsch_queue_update_time_source(&destination);\n'
    str_max_slot_frame_used           = 'max_slot_frame_used = {};\n'

    str_start_mat                     = 'const uint8_t available_slot_matrix[] = {\n'
    str_mat                           = '    {}, {}, {}, {}, {}, \n'                       # node_id , TX_slot, TX_channel, RX_slot, RX_channel
    str_end_mat                       = '};\n'

    str_func_call                     = 'copy_available_slot_matrix(available_slot_matrix);\n'

    str_end                           = '}\n'
                          

    self.node_schedule = self.schedule.get_node_schedule(*self.node_ids)

    if with_timesource:
      self.get_timesource_for_nodes()

    # absolute_path = "/home/ptm/Documents/Design and Evaluation of Deadline-Based Scheduling Algorithms for the Industrial Internet of Things/software/MASTER/MASTER/contiki-ng/"
    absolute_path = str(dirname(dirname(dirname(abspath(__file__)))))
    # macro file for MASTER (bookkeeping)
    scheduling_macros_h = open(absolute_path + "/examples/master/master-unicast/scheduling_macros.h", 'w')
    scheduling_macros_h.write("#ifndef __SCHEDULING_MACROS_H__\n")
    scheduling_macros_h.write("#define __SCHEDULING_MACROS_H__\n\n")
    # file to adjust the send interval of each sender
    scheduling_send_interval_c = open(absolute_path + "/examples/master/master-unicast/scheduling_send_interval.c", 'w')
    scheduling_send_interval_c.write("#ifdef CONTIKI_TARGET_SKY\n  ")
    flows = self.schedule.flows.copy()
    flows.sort(key = lambda x: x.source, reverse = True)
    while flows:
      flow = flows.pop()
      flows = list(filter(lambda x: not x.source == flow.source, flows))
      scheduling_send_interval_c.write("if (node_id == " + str(flow.source) + ") {\n")
      scheduling_send_interval_c.write("    etimer_set(&periodic_timer, ({} * CLOCK_SECOND) / 40);\n".format(flow.period))
      scheduling_send_interval_c.write("  }" + (" else ", "\n") [flows == []])
    scheduling_send_interval_c.write("#else\n  ")
    flows = self.schedule.flows.copy()
    flows.sort(key = lambda x: x.source, reverse = True)
    while flows:
      flow = flows.pop()
      flows = list(filter(lambda x: not x.source == flow.source, flows))
      scheduling_send_interval_c.write("if (node_id == " + str(flow.source) + ") {\n")
      scheduling_send_interval_c.write("    etimer_set(&periodic_timer, ({} * CLOCK_SECOND) / 100);\n".format(flow.period))
      scheduling_send_interval_c.write("  }" + (" else ", "\n") [flows == []])
    scheduling_send_interval_c.write("#endif")
    scheduling_send_interval_c.close()

    output_file = open(output_file_path, 'w')
    first_node = True
    output_file.write( str_func ) ## start function
    slotframe_length = len(self.schedule.schedule[0])
    add_beacon_slot = False
    if slotframe_length < minimal_schedule_length:
      slotframe_length = minimal_schedule_length
      add_beacon_slot = True
    # if slotframe_length % 3 == 0: # if divisible by three
    #   add_beacon_slot = True
    #   slotframe_length += 1
    if len(self.schedule.flows) > max(self.node_ids):
      scheduling_macros_h.write("#undef MASTER_MAX_SLOTFRAMES\n")
      scheduling_macros_h.write("#define MASTER_MAX_SLOTFRAMES {}\n".format(len(self.schedule.flows)))
    output_file.write( str_schedule_length.format(slotframe_length) )

    # create slotframes: how many do we need? num flows!
    flows = self.schedule.flows.copy()
    while flows:
      flows.sort(key = lambda x: x.flow_number)
      for flow in flows[1:]:
        if flows[0].flow_number == flow.flow_number:
          flows.remove(flow)
      output_file.write(str_create_slotframe.format(flows[0].flow_number))
      flows.remove(flows[0])

    # init last received packet
    flows = self.schedule.flows.copy()
    while flows:
      flows.sort(key = lambda x: x.flow_number)
      for flow in flows[1:]:
        if flows[0].flow_number == flow.flow_number:
          flows.remove(flow)
      output_file.write(str_last_received_packet_of_flow.format(flows[0].flow_number))
      flows.remove(flows[0])

    flows = self.schedule.flows.copy()
    while flows:
      flows.sort(key = lambda x: x.flow_number)
      for flow in flows[1:]:
        if flows[0].flow_number == flow.flow_number:
          flows.remove(flow)
      output_file.write(str_flow_sender.format(flows[0].flow_number, flows[0].source))
      output_file.write(str_flow_receiver.format(flows[0].flow_number, flows[0].destination))
      flows.remove(flows[0])

    output_file.write( str_ttl_if )
    flows = self.schedule.flows.copy()
    max_tx_size = 0
    while flows:
      flows.sort(key = lambda x: x.flow_number)
      sub_flows = list(filter(lambda x: x.flow_number == flows[0].flow_number, flows))
      sub_flows.sort(key = lambda x: x.cells[0].timeslot)
      if max_tx_size < len(sub_flows):
        max_tx_size = len(sub_flows)
      for (index, sub_flow) in enumerate(sub_flows):
        output_file.write(str_first_tx_slot_of_sf.format(sub_flow.flow_number - 1, index, sub_flow.cells[0].timeslot))
        output_file.write(str_last_tx_slot_of_sf.format(sub_flow.flow_number - 1, index, sub_flow.cells[-1].timeslot))
        flows.remove(sub_flow)
    # macro file for MASTER (bookkeeping)
    scheduling_tx_macros_h = open(absolute_path + "/os/net/master-routing/scheduling_tx_macros.h", 'w')
    scheduling_tx_macros_h.write("#ifndef __SCHEDULING_TX_MACROS_H__\n")
    scheduling_tx_macros_h.write("#define __SCHEDULING_TX_MACROS_H__\n\n")
    scheduling_tx_macros_h.write("  #define MAX_TX_SIZE {}\n\n".format(max_tx_size))
    scheduling_tx_macros_h.write("#endif")
    scheduling_tx_macros_h.close()
    output_file.write( str_ttl_endif )

    max_number_links_per_node = 0
    for node_id in [*self.node_schedule]:

      if first_node:
        output_file.write( str_start_first_node.format(node_id) )
        first_node = False
      else:
        output_file.write( str_start_next_node.format(node_id) )

      sending_flow_of_node = self.get_flow_from_sender(node_id)
      if sending_flow_of_node:
        output_file.write( str_own_tx_flow.format(sending_flow_of_node.flow_number) )
        output_file.write( str_own_receiver.format(sending_flow_of_node.destination) )
        output_file.write("  schedule_length = {};\n".format(list(filter(lambda x: x.flow_number == sending_flow_of_node.flow_number, self.schedule.flows))[0].period))

      links = self.sorted_list_of_links(node_id)
      if len(links) > max_number_links_per_node:
        max_number_links_per_node = len(links)

      output_file.write( str_start_links )

      last_neighbor = None
      change_neighbor_on_link_index = []
      forward_to = []
      participating_flows_as_sender = []

      for link_index, link in enumerate(links):
        if link.neighbor != last_neighbor:
          change_neighbor_on_link_index.append( (link_index, link.neighbor) )
          last_neighbor = link.neighbor

        output_file.write( str_link.format(str(link.flow_number).rjust(2), str(link.link_option).rjust(2), str(link.timeslot).rjust(2), str(link.channel).rjust(2)) )

        if link.link_option != Link_options.receive.value: # not a receive-only link
          forward_to.append( (link.flow_number, link.neighbor) )
          if link.flow_number not in participating_flows_as_sender:
            participating_flows_as_sender.append(link.flow_number)

      output_file.write( str_end_links )

      forward_to = list(dict.fromkeys(forward_to))
      for flow_number, neighbor in forward_to:
        output_file.write( str_forward_to.format(flow_number, neighbor) )

      str_change_indices = ''
      str_change_indices_to = ''
      for link_index, neighbor in change_neighbor_on_link_index:
        str_change_indices += str(link_index) + ', '
        str_change_indices_to += str(neighbor) + ', '
      output_file.write( str_change_on_index.format(str_change_indices) )
      output_file.write( str_change_on_index_to.format(str_change_indices_to) )

      output_file.write( str_links_call.format(len(links), len(change_neighbor_on_link_index)) )

      if with_timesource and node_id != self.network_time_source:
        output_file.write( str_set_timesource.format(self.timesource[node_id]) )

      for flow_number in participating_flows_as_sender:
        flow = self.schedule.get_flow_from_id(flow_number)
        if node_id in flow.max_transmissions:
          output_file.write( str_max_transmissions.format(flow_number - 1, flow.max_transmissions[node_id]) )

      if sending_flow_of_node:
        output_file.write( str_not_ttl_if )
        sending_slots = sending_flow_of_node.get_timeslots_of_source()
        for idx, timeslot in enumerate(sending_slots):
          output_file.write( str_sending_slots.format(idx, timeslot) )
        output_file.write( str_num_sending_slots.format(len(sending_slots)) )
        output_file.write( str_not_ttl_endif )

    # timeslots of non-participating nodes:
    if with_timesource:
      non_scheduled_nodes = set(self.node_ids) - set([*self.node_schedule])
      for node_id in non_scheduled_nodes:
        if node_id != self.network_time_source:
          output_file.write( str_start_next_node.format(node_id) )
          output_file.write( str_set_timesource.format(self.timesource[node_id]) )

    output_file.write( str_end_last_node )

    if add_beacon_slot:
      output_file.write( str_beacon_link.format(slotframe_length - 1) ) # last slot
      
    max_slot_used = 0
    flows = self.schedule.flows.copy()
    for flow in flows:
      max_slot_used = flow.flow_number

    output_file.write( str_max_slot_frame_used.format(max_slot_used))

    # rows = []
    # for link_index, link in enumerate(links):
    #     if link.neighbor != last_neighbor:
    #       change_neighbor_on_link_index.append( (link_index, link.neighbor) )
    #       last_neighbor = link.neighbor

    #     output_file.write( str_link.format(str(link.flow_number).rjust(2), str(link.link_option).rjust(2), str(link.timeslot).rjust(2), str(link.channel).rjust(2)) )

    output_file.write(str_start_mat)
    for index,row in enumerate(self.schedule.available_slot_matrix):
      output_file.write( str_mat.format(str(row[0]).rjust(2), str(row[1]).rjust(2), str(row[2]).rjust(2), str(row[3]).rjust(2), str(row[4]).rjust(2)) )
    output_file.write(str_end_mat)

    output_file.write(str_func_call)
    output_file.write(str_end)
    output_file.close()

    # print("TheKing--> ", self.schedule.used_slot_matrix)



    scheduling_macros_h.write("#undef TSCH_SCHEDULE_CONF_MAX_LINKS\n")
    scheduling_macros_h.write("#define TSCH_SCHEDULE_CONF_MAX_LINKS {}\n\n".format(max_number_links_per_node + 2))
    scheduling_macros_h.write("#endif")
    scheduling_macros_h.close()
    print("TSCH_SCHEDULE_CONF_MAX_LINKS {}".format(max_number_links_per_node+2))
