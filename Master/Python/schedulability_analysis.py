#!/usr/local/bin/python3

from cell import Cell
from cell_c_llf import Cell_C_LLF
from flow import Flow
from neighbor_parser import Parser
from scheduling_configuration import Scheduling_strategies, Scheduling_window_size_algorithm

import copy
from math import gcd
import random
import sys

def create_schedule_flows(raw_flows):
    shortest_repeating_cycle = 0

    if len(raw_flows) >= 1:
        shortest_repeating_cycle = raw_flows[0][2]
    for index_idx in range(1, len(raw_flows)):
        shortest_repeating_cycle = int(shortest_repeating_cycle * raw_flows[index_idx][2] / gcd(shortest_repeating_cycle, raw_flows[index_idx][2]))

    neighbor_parser = Parser([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21], "/home/ptm/Documents/Design and Evaluation of Deadline-Based Scheduling Algorithms for the Industrial Internet of Things/software/MASTER/schedule/evaluation/software/", "master-neighbor-discovery.log", 10)
    neighbor_parser.parse_neighbor_data(None, None, None)

    schedule_flows = []

    flow_id = 1
    sub_flow_id = 1
    for raw_flow in raw_flows:
        source = raw_flow[0]
        destination = raw_flow[1]
        period = raw_flow[2]
        release_time = raw_flow[3]
        if release_time > 0:
            release_time -= 1
        else:
            release_time = 0
        deadline = raw_flow[4]
        for period_idx in range(0, shortest_repeating_cycle):
            if period_idx * period >= shortest_repeating_cycle:
                break
            schedule_flows.append(Flow(neighbor_parser.graph_etx, flow_id, sub_flow_id, source, destination, period, release_time + period_idx * period, deadline))
            sub_flow_id += 1
        flow_id += 1
    return (schedule_flows, shortest_repeating_cycle)

def create_schedule(schedule_flows, shortest_repeating_cycle):
    max_schedule_length = 0
    max_time_slot = 0
    for flow in schedule_flows:
        flow.compute_path(2)
        max_schedule_length += flow.length
        flow.create_cells(Scheduling_strategies.sliding_window, Scheduling_window_size_algorithm.etx_link_based_doubled, None)
        flow.set_CNMID(False)
        if max_time_slot < flow.release_time:
            max_time_slot = flow.release_time
        if (shortest_repeating_cycle > 1 and flow.release_time > shortest_repeating_cycle) or (not shortest_repeating_cycle == 1 and flow.release_time >= shortest_repeating_cycle):
            flow.release_time %= shortest_repeating_cycle
    schedule = [[0] * (max_schedule_length * shortest_repeating_cycle + max_time_slot) for _ in range(4)]
    used_nodes_in_timeslot = [[] for _ in range(len(schedule[0]))]
    return (schedule, used_nodes_in_timeslot)

def find_schedulable_slot(schedule, cell, release_time_idx):
    for timeslot_idx in range(len(schedule[0])):
        for channel_idx in range(len(schedule)):
            if isinstance(schedule[channel_idx][timeslot_idx], Cell):
                if not set(cell.participants).intersection(set(schedule[channel_idx][timeslot_idx].participants)) == set():
                    break
            if schedule[channel_idx][timeslot_idx] == 0 and timeslot_idx >= release_time_idx:
                return (channel_idx, timeslot_idx)
    return None

def scheduler_EDF(schedule_flows, shortest_repeating_cycle, schedule, schedulability_analysis_EDF_log):
    schedule_flows.sort(key = lambda x: x.deadline)
    for flow in schedule_flows:
        release_time = flow.release_time
        for cell in flow.cells:
            schedule_idx = find_schedulable_slot(schedule, cell, release_time)
            if schedule_idx:
                if schedule_idx[1] >= flow.release_time + flow.deadline or (not shortest_repeating_cycle == 1 and schedule_idx[1] + 1 > shortest_repeating_cycle):
                    schedulability_analysis_EDF_log.write("0")
                    return
                schedule[schedule_idx[0]][schedule_idx[1]] = cell
                release_time = schedule_idx[1] + 1
            else:
                raise ValueError("error@schedulability_analysis.py: flow {" + "{} -> {}".format(flows.source, flow.destination) + "} is faulty!")
    schedulability_analysis_EDF_log.write("1")

def scheduler_RMS(schedule_flows, shortest_repeating_cycle, schedule, schedulability_analysis_RMS_log):
    schedule_flows = sorted(schedule_flows, key = lambda x: (x.period, x.deadline))
    for flow in schedule_flows:
        release_time = flow.release_time
        for cell in flow.cells:
            schedule_idx = find_schedulable_slot(schedule, cell, release_time)
            if schedule_idx:
                if schedule_idx[1] >= flow.release_time + flow.deadline or (not shortest_repeating_cycle == 1 and schedule_idx[1] + 1 > shortest_repeating_cycle):
                    schedulability_analysis_RMS_log.write("0")
                    return
                schedule[schedule_idx[0]][schedule_idx[1]] = cell
                release_time = schedule_idx[1] + 1
            else:
                raise ValueError("error@schedulability_analysis.py: flow {" + "{} -> {}".format(flows.source, flow.destination) + "} is faulty!")
    schedulability_analysis_RMS_log.write("1")

def compute_laxity(cell_c_llf, to_release, timeslot_idx):
    intersection_flows = list(filter(lambda x: cell_c_llf.flow.release_time <= x.flow.release_time <= cell_c_llf.flow.deadline and not set(x.cell.participants).intersection(set([cell_c_llf.flow.source])) == set(), to_release))

    critical_window = []
    for flow in intersection_flows:
        critical_window.append(((flow.flow.deadline - timeslot_idx + 1) - len(list(filter(lambda x: timeslot_idx <= x.flow.release_time and x.flow.deadline <= flow.flow.deadline, intersection_flows)))))
    if not critical_window == []:
        cell_c_llf.laxity = min(critical_window)

def scheduler_C_LLF(schedule_flows, shortest_repeating_cycle, schedule, schedulability_analysis_C_LLF_log):
    transmissions_flows = []
    for flow in schedule_flows:
        for cell in flow.cells:
            transmissions_flows.append(Cell_C_LLF(flow, cell))
    timeslot_idx = 0
    while transmissions_flows:
        to_release = list(filter(lambda x: x.flow.release_time <= timeslot_idx, transmissions_flows))
        for transmissions_flow in transmissions_flows:
            compute_laxity(transmissions_flow, to_release, timeslot_idx)
        channel_idx = 0
        while channel_idx < len(schedule) and to_release:
            to_release.sort(key = lambda x: x.laxity)
            transmission = list(filter(lambda x: x.laxity == to_release[0].laxity, to_release))
            transmission.sort(key = lambda x: x.flow.deadline)
            release_time = transmission[0].flow.release_time
            for cell in transmission[0].flow.cells:
                schedule_idx = find_schedulable_slot(schedule, cell, release_time)
                if schedule_idx:
                    if schedule_idx[1] >= transmission[0].flow.release_time + transmission[0].flow.deadline or (not shortest_repeating_cycle == 1 and schedule_idx[1] + 1 > shortest_repeating_cycle):
                        schedulability_analysis_C_LLF_log.write("0")
                        return
                    schedule[schedule_idx[0]][schedule_idx[1]] = cell
                    release_time = schedule_idx[1] + 1
                    transmissions_flows = list(filter(lambda x: not (cell == x.cell and transmission[0].flow.sub_flow_number == x.flow.sub_flow_number), transmissions_flows))
            participants = []
            for cell in transmission[0].flow.cells:
                participants.append(cell.participants)
            to_release = list(filter(lambda x: set(x.cell.participants).intersection() == set(participants), to_release))
            channel_idx += 1
        timeslot_idx += 1
    schedulability_analysis_C_LLF_log.write("1")

def schedulable_in_slot(schedule, used_nodes_in_timeslot, slot_index, participants):
    slot_available = False
    for channel in schedule:
        if channel[slot_index] == 0:
            slot_available = True
    if not slot_available:
        return False
    if len(used_nodes_in_timeslot[slot_index]) == 0:
        return True
    for participant in participants:
        if participant in used_nodes_in_timeslot[slot_index]:
            return False
    return True

def available_channel_in_slot(schedule, slot_index):
    for channel_idx in range(len(schedule)):
        if schedule[channel_idx][slot_index] == 0:
            return channel_idx
    return False

def scheduler_R_LPF(schedule_flows, shortest_repeating_cycle, schedule, used_nodes_in_timeslot, schedulability_analysis_R_LPF_log):
    schedule_flows.sort(key = lambda x: x.length, reverse = True)
    current_flow_number = -1
    current_idx = 0
    for flow in schedule_flows:
        insert_at_index = 0
        if current_flow_number == -1 or not current_flow_number == flow.flow_number:
            current_flow_number = flow.flow_number
            current_idx = 0
        for cell in reversed(flow.cells):
            while not schedulable_in_slot(schedule, used_nodes_in_timeslot, insert_at_index, cell.participants) or insert_at_index < current_idx:
                insert_at_index += 1
            channel = available_channel_in_slot(schedule, insert_at_index)
            if insert_at_index >= flow.release_time + flow.deadline or (not shortest_repeating_cycle == 1 and insert_at_index + 1 > shortest_repeating_cycle):
                schedulability_analysis_R_LPF_log.write("0")
                return
            schedule[channel][insert_at_index] = cell
            used_nodes_in_timeslot[insert_at_index].extend(cell.participants)
        current_idx += flow.period
    schedulability_analysis_R_LPF_log.write("1")

if __name__ == "__main__":
    absolute_path = "/home/ptm/Documents/Design and Evaluation of Deadline-Based Scheduling Algorithms for the Industrial Internet of Things/software/MASTER/schedule/evaluation/software/schedulability analysis/"
    schedulability_analysis_log = open(absolute_path + "schedulability_analysis.log", 'w')
    schedulability_analysis_EDF_log = open(absolute_path + "schedulability_analysis_EDF.log", 'w')
    schedulability_analysis_RMS_log = open(absolute_path + "schedulability_analysis_RMS.log", 'w')
    schedulability_analysis_C_LLF_log = open(absolute_path + "schedulability_analysis_C-LLF.log", 'w')
    schedulability_analysis_R_LPF_log = open(absolute_path + "schedulability_analysis_R-LPF.log", 'w')

    MAX_ROUNDS = 500
    to_exclude = [11]
    flow_sequence = [1, 2, 4, 8, 16, 32]
    flow_period = [32, 64, 128, 256]

    print("--------------------------------------------------")
    print("analysis: running!")
    print("--------------------------------------------------\n")
    for (sequence_idx, sequence) in enumerate(flow_sequence):
        print("current sequence: {} of {}.".format(sequence, flow_sequence))
        for round_idx in range(0, MAX_ROUNDS):
            if round_idx == 0:
                print("")
            print("current round: {} of {}.\n".format(round_idx + 1, MAX_ROUNDS))
            raw_flows = []
            for _ in range(0, sequence):
                sender = random.randint(1, 21)
                receiver = random.randint(1, 21)
                while sender in to_exclude or receiver in to_exclude or sender == receiver:
                    sender = random.randint(1, 21)
                    receiver = random.randint(1, 21)
                period = flow_period[random.randint(0, len(flow_period) - 1)]
                release_time = 0 # default: random.randint(0, 256)
                deadline = random.randint(1, 256)
                raw_flows.append((sender, receiver, period, release_time, deadline))
            (schedule_flows, shortest_repeating_cycle) = create_schedule_flows(raw_flows)
            for (index, schedule_flow) in enumerate(schedule_flows):
                schedulability_analysis_log.write(schedule_flow.__str__())
                if not index + 1 == len(schedule_flows):
                    schedulability_analysis_log.write("\n")
            (schedule, used_nodes_in_timeslot) = create_schedule(schedule_flows, shortest_repeating_cycle)
            scheduler_EDF(copy.deepcopy(schedule_flows), shortest_repeating_cycle, copy.deepcopy(schedule), schedulability_analysis_EDF_log)
            scheduler_RMS(copy.deepcopy(schedule_flows), shortest_repeating_cycle, copy.deepcopy(schedule), schedulability_analysis_RMS_log)
            scheduler_C_LLF(copy.deepcopy(schedule_flows), shortest_repeating_cycle, copy.deepcopy(schedule), schedulability_analysis_C_LLF_log)
            scheduler_R_LPF(copy.deepcopy(schedule_flows), shortest_repeating_cycle, copy.deepcopy(schedule), used_nodes_in_timeslot, schedulability_analysis_R_LPF_log)
            if not round_idx + 1 == MAX_ROUNDS:
                schedulability_analysis_log.write("\n---------\n")
                schedulability_analysis_EDF_log.write(" | ")
                schedulability_analysis_RMS_log.write(" | ")
                schedulability_analysis_C_LLF_log.write(" | ")
                schedulability_analysis_R_LPF_log.write(" | ")
        if not sequence_idx + 1 == len(flow_sequence):
            schedulability_analysis_log.write("\n--------------------------------------------------------------------------------------------------------------------------------------------\n")
            schedulability_analysis_EDF_log.write("\n")
            schedulability_analysis_RMS_log.write("\n")
            schedulability_analysis_C_LLF_log.write("\n")
            schedulability_analysis_R_LPF_log.write("\n")
            print("--------------------------------------------------\n")
    print("--------------------------------------------------")
    print("analysis: done!")
    print("--------------------------------------------------")
    schedulability_analysis_log.close()
    schedulability_analysis_EDF_log.close()
    schedulability_analysis_RMS_log.close()
    schedulability_analysis_C_LLF_log.close()
    schedulability_analysis_R_LPF_log.close()