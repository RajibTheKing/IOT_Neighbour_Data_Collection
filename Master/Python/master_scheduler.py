#!/usr/local/bin/python3

import sys
import argparse
import project_configuration as config
from neighbor_parser import Parser
from flow import Flow
from scheduling_configuration import Scheduling_algorithm, Scheduling_strategies, Scheduling_window_size_algorithm
from scheduling import Schedule
from scheduling_contiki import Contiki_schedule

import copy
from math import gcd

def main():
  parser = argparse.ArgumentParser(description='Scheduler')
  parser.add_argument('-dir', '--folder', help='Folder containing neighbor discovery statistics', required=True)
  parser.add_argument('-f', '--file', help='File in specified folder, include iff 1 file')
  parser.add_argument('-flows_f', '--flows_file', help='File of comma seperated values with to be scheduled flows, lower priority than --flows')
  parser.add_argument('-flows', '--flows', help='String of to be scheduled flows; "<from>,<to>,<period>[,<release_time>[,<deadline>]];..."')
  parser.add_argument('-tb', '--testbed', help='Testbed, default: cooja', choices=['cooja', 'flocklab', 'kiel'])
  parser.add_argument('-n_cooja', '--num_cooja_nodes', help='Number of cooja nodes, default: 5', type=int)
  parser.add_argument('-trg', '--target', help='Target platform, default: sky', choices=['zoul', 'sky', 'cooja'])
  parser.add_argument('-alg', '--algorithm', help='Scheduling algorithm, default: R-LPF', choices=['R-LPF', 'EDF', 'RMS', 'C-LLF'])
  parser.add_argument("-ign_NSF", "--ignore_NSF", help = "ignore not schedulable flows, default: infeasible.", choices = ["enabled", "disabled", "infeasible"])
  parser.add_argument('-strat', '--strategy', help='(Re)Transmission strategy, default: sliding', choices=['no', 'slot', 'sliding'])
  parser.add_argument('-salg', '--sliding_algorithm', help='Sliding Windows algorithm, default: etx_link_based',
                      choices=['etx_based', 'etx_based_doubled', 'etx_based_trippled',
                               'etx_link_based', 'etx_link_based_doubled', 'etx_link_based_trippled', 'fixed']
                     )
  parser.add_argument('-wsize', '--fixed_window_size', help="Fixed window size for sliding algorithm 'fixed' (minimum = 2), default: 5", type=int)
  parser.add_argument('-etx', '--etx_power', help='ETX-Power, default: 2', type=int, choices=[1, 2, 3])
  parser.add_argument('-ch', '--channels', help='Number channels, default: 4', type=int, choices=range(1,17))
  parser.add_argument('-m_etx', '--max_etx', help='Maximal accepted ETX value, default: 10', type=int)
  parser.add_argument('-with_cs', '--with_contiki_schedule', help='Generate Contiki Schedule, default: False', action='store_true')
  parser.add_argument('-ts', '--timesource', help='Time Source node of network, default: 1', type=int)
  parser.add_argument('-out', '--output_file', help='Output File of Contiki Schedule')
  parser.add_argument('-m_len', '--minimal_length', help='Minimal Schedule Length, default: minimal possible', type=int)
  parser.add_argument('-with_ts', '--with_scheduled_timesource', help='Schedule timesource for each node, default: False', action='store_true')
  parser.add_argument('-p_etx', '--print_etx', help='Print ETX table', action='store_true')
  parser.add_argument('-p_prr', '--print_prr', help='Print PRR table', action='store_true')
  parser.add_argument('-p_rssi', '--print_rssi', help='Print RSSI table', action='store_true')
  parser.add_argument('-p_sched', '--print_schedule', help='Print Schedule', action='store_true')
  parser.add_argument('-sv', '--print_strength_vector', help='Use already calculated strength vector to generate ETX/PRR', action='store_true')
  args = parser.parse_args()

  folder = args.folder
  fixed_window_size = None

  if args.file:
    filename = args.file

  if args.testbed:
    if args.testbed == 'cooja':
      testbed = config.Testbed.cooja
    elif args.testbed == 'flocklab':
      testbed = config.Testbed.flocklab
    elif args.testbed == 'kiel':
      testbed = config.Testbed.kiel
  else:
    testbed = config.Testbed.cooja

  if testbed == config.Testbed.cooja:
    if args.num_cooja_nodes:
      num_cooja_nodes = args.num_cooja_nodes
    else:
      num_cooja_nodes = 5
    node_ids = config.get_valid_node_ids(testbed, num_cooja_nodes)
  else:
    node_ids = config.get_valid_node_ids(testbed)

  if args.target:
    if args.target == 'zoul':
      target = config.Platform.zoul
    elif args.target == 'sky':
      target = config.Platform.sky
    elif args.target == 'cooja':
      target = config.Platform.cooja
  else:
    target = config.Platform.sky

  if args.algorithm:
    if args.algorithm.upper() == 'R-LPF':
      scheduling_algorithm = Scheduling_algorithm.R_LPF
    if args.algorithm.upper() == 'EDF':
      scheduling_algorithm = Scheduling_algorithm.EDF
    if args.algorithm.upper() == 'RMS':
      scheduling_algorithm = Scheduling_algorithm.RMS
    if args.algorithm.upper() == 'C-LLF':
      scheduling_algorithm = Scheduling_algorithm.C_LLF
  else:
    scheduling_algorithm = Scheduling_algorithm.R_LPF

  if args.ignore_NSF:
    if args.ignore_NSF == "enabled":
      scheduling_ignore_NSF = "enabled"
    if args.ignore_NSF == "disabled":
      scheduling_ignore_NSF = "disabled"
    if args.ignore_NSF == "infeasible":
      scheduling_ignore_NSF = "infeasible"
  else:
    scheduling_ignore_NSF = "infeasible"

  if args.strategy:
    if args.strategy == 'no':
      scheduling_strategy = Scheduling_strategies.no_retransmissions
    elif args.strategy == 'slot':
      scheduling_strategy = Scheduling_strategies.upper_etx_tx_per_link
    elif args.strategy == 'sliding':
      scheduling_strategy = Scheduling_strategies.sliding_window
  else:
    scheduling_strategy = Scheduling_strategies.sliding_window

  if args.sliding_algorithm:
    if args.sliding_algorithm == 'etx_based':
      scheduling_window_size_alg = Scheduling_window_size_algorithm.etx_based
    elif args.sliding_algorithm == 'etx_based_doubled':
      scheduling_window_size_alg = Scheduling_window_size_algorithm.etx_based_doubled
    elif args.sliding_algorithm == 'etx_based_trippled':
      scheduling_window_size_alg = Scheduling_window_size_algorithm.etx_based_trippled
    elif args.sliding_algorithm == 'etx_link_based':
      scheduling_window_size_alg = Scheduling_window_size_algorithm.etx_link_based
    elif args.sliding_algorithm == 'etx_link_based_doubled':
      scheduling_window_size_alg = Scheduling_window_size_algorithm.etx_link_based_doubled
    elif args.sliding_algorithm == 'etx_link_based_trippled':
      scheduling_window_size_alg = Scheduling_window_size_algorithm.etx_link_based_trippled
    elif args.sliding_algorithm == 'fixed':
      scheduling_window_size_alg = Scheduling_window_size_algorithm.fixed
      if args.fixed_window_size:
        if args.fixed_window_size < 2:
          raise argparse.ArgumentTypeError("Minimum window size is 2")
        else:
          fixed_window_size = args.fixed_window_size
      else:
        fixed_window_size = 5
  else:
    scheduling_window_size_alg = Scheduling_window_size_algorithm.etx_link_based

  if args.etx_power:
    etx_power = args.etx_power
  else:
    etx_power = 2

  if args.channels:
    num_channels = args.channels
  else:
    num_channels = 4

  if args.max_etx:
    max_etx = args.max_etx
  else:
    max_etx = 10

  if args.with_contiki_schedule:
    generate_contiki_schedule = True
    if args.output_file:
      contiki_output_file = args.output_file
    else:
      raise ValueError("Please specify the Contiki output file!")

    if args.minimal_length:
      contiki_minimal_schedule_length = args.minimal_length
    else:
      contiki_minimal_schedule_length = 0

    if args.with_scheduled_timesource:
      contiki_schedule_timesource = True
    else:
      contiki_schedule_timesource = False
  else:
    generate_contiki_schedule = False

  if args.timesource and args.timesource in node_ids:
    network_time_source = args.timesource
  else:
    network_time_source = 1

  communications = []
  if args.flows:
    flows_to_create = args.flows.split(';')
    for flow_to_create in flows_to_create:
      flow_parameters = [int(x.strip()) for x in flow_to_create.split(',')]
      communications.append(tuple(flow_parameters[:5]))
  elif args.flows_file:
    with open(args.flows_file) as flows_file:
      for line in flows_file:
        flow_parameters = [int(x.strip()) for x in line.split(',')]
        communications.append(tuple(flow_parameters[:5]))
  # else:
  #   communications.append( (1, 2, 1) )


  # Parse neighbor discovery file(s)
  neighbor_parser = Parser(node_ids, folder, filename, max_etx)
  neighbor_parser.parse_neighbor_data(args.print_etx, args.print_prr, args.print_rssi, args.print_strength_vector)
  ##neighbor_parser.create_DGRM_configuration()
  if communications == []:
    raise ValueError("error@master_scheduler.py: one or more flow(s) missing!")

  # BEGIN OF SAFTY MODULE:

  # user entered one or more invalid source node(s)
  invalid_SOURCE_and_destination = []
  # user entered one or more invalid destination node(s)
  invalid_source_and_DESTINATION = []
  # user entered one or more invalid source and destination node(s)
  invalid_source_and_destination = []
  # user not entered one or more destination node(s)
  missing_destination = []
  # user entered one or more invalid period(s) (negative or floating-point value(s))
  invalid_periods = []
  # user not entered one ore more period(s)
  missing_periods = []
  # user entered one or more invalid release time(s) (negative or floating-point value(s))
  invalid_release_time = []
  # user entered one or more invalid deadline(s) (negative or floating-point value(s))
  invalid_deadline = []

  # do for all flows:
  for communication in communications:
    # are there flows?
    if len(communication) > 0:
      # is/are there (a) invalid source node(s)?
      if communication[0] <= 0:
        # if the source node is invalid, is a destination node available for a better error output?
        if len(communication) > 1:
          invalid_SOURCE_and_destination.append((communication[0], communication[1]))
        else:
          invalid_SOURCE_and_destination.append((communication[0], "????"))
    else:
      raise ValueError("error@master_scheduler.py: missing source and destination nodes!")
    # is/are there (a) destination(s)?
    if len(communication) > 1:
      # is/are there (a) invalid destination node(s)?
      if communication[1] <= 0:
        invalid_source_and_DESTINATION.append((communication[0], communication[1]))
    else:
      missing_destination.append(communication[0])
      continue
    # is/are there (a) period(s)?
    if len(communication) > 2:
      # is/are there (a) invalid period node(s)?
      if communication[2] <= 0:
        invalid_periods.append((communication[0], communication[1], communication[2]))
    else:
      missing_periods.append((communication[0], communication[1]))
      continue
    # source and destination are the same
    if communication[0] == communication[1]:
      invalid_source_and_destination.append((communication[0], communication[1]))
    # is/are there (a) release time(s)?
    if len(communication) > 3:
      # is/are there (a) invalid release time(s)?
      if communication[3] < 0:
        invalid_release_time.append((communication[0], communication[1], communication[3]))
    # is/are there (a) deadline(s)?
    if len(communication) > 4:
      # is/are there (a) invalid deadline(s)?
      if communication[4] <= 0:
        invalid_deadline.append((communication[0], communication[1], communication[4]))

  # output of all error flows with a specific error message:

  if not invalid_SOURCE_and_destination == []:
    raise ValueError("error@master_scheduler.py: the following flow" + ("", "s") [len(invalid_SOURCE_and_destination) > 1] + " {" + " | ".join("({:2d} -> {:2d}, {:2d})".format(source, destination, source) for (source, destination) in invalid_SOURCE_and_destination) + "} " + ("has", "have") [len(invalid_SOURCE_and_destination) > 1] + " an invalid source!")
  if not invalid_source_and_DESTINATION == []:
    raise ValueError("error@master_scheduler.py: the following flow" + ("", "s") [len(invalid_source_and_DESTINATION) > 1] + " {" + " | ".join("({:2d} -> {:2d}, {:2d})".format(source, destination, destination) for (source, destination) in invalid_source_and_DESTINATION) + "} " + ("has", "have") [len(invalid_source_and_DESTINATION) > 1] + " an invalid destination!")
  if not invalid_source_and_destination == []:
    raise ValueError("error@master_scheduler.py: the following flow" + ("", "s") [len(invalid_source_and_destination) > 1] + " {" + " | ".join("({:2d} -> {:2d})".format(source, destination) for (source, destination) in invalid_source_and_destination) + "} " + ("has", "have") [len(invalid_source_and_destination) > 1] + " the same source and destination!")
  if not missing_destination == []:
    raise ValueError("error@master_scheduler.py: the following flow" + ("", "s") [len(missing_destination) > 1] + " {" + " | ".join("({:2d} -> ????)".format(source) for source in missing_destination) + "} " + ("has", "have") [len(missing_destination) > 1] + " an missing destination!")
  if not invalid_periods == []:
    raise ValueError("error@master_scheduler.py: the following flow" + ("", "s") [len(invalid_periods) > 1] + " {" + " | ".join("({:2d} -> {:2d}, {:4d})".format(source, destination, period) for (source, destination, period) in invalid_periods) + "} " + ("has", "have") [len(invalid_periods) > 1] + " an invalid period!")
  if not missing_periods == []:
    raise ValueError("error@master_scheduler.py: the following flow" + ("", "s") [len(missing_periods) > 1] + " {" + " | ".join("({:2d} -> {:2d})".format(source, destination) for (source, destination) in missing_periods) + "} " + ("has", "have") [len(missing_periods) > 1] + " an missing period!")
  if not invalid_release_time == []:
    raise ValueError("error@master_scheduler.py: the following flow" + ("", "s") [len(invalid_release_time) > 1] + " {" + " | ".join("({:2d} -> {:2d}, {:4d})".format(source, destination, release_time) for (source, destination, release_time) in invalid_release_time) + "} " + ("has", "have") [len(invalid_release_time) > 1] + " an invalid release time!")
  if not invalid_deadline == []:
    raise ValueError("error@master_scheduler.py: the following flow" + ("", "s") [len(invalid_deadline) > 1] + " {" + " | ".join("({:2d} -> {:2d}, {:4d})".format(source, destination, deadline) for (source, destination, deadline) in invalid_deadline) + "} " + ("has", "have") [len(invalid_deadline) > 1] + " an invalid deadline!")

  # END OF SAFTY MODULE

  THRESHOLD_HYPER_PERIOD = 20
  shortest_repeating_cycle = 0

  # determines the hyper-period through calculating: m * n / gcd(m, n)
  if len(communications) >= 1:
    if communications[0][2] <= THRESHOLD_HYPER_PERIOD:
      print("warning@master_scheduler.py: for flow {" + "({:2d} -> {:2d})".format(communications[0][0], communications[0][1]) + "} the period (recommend: > " + "{:4d}, but found: ".format(THRESHOLD_HYPER_PERIOD) + "{:4d}".format(communications[0][2]) + ") is maybe too small to work probably for a simulated network!\n")
    shortest_repeating_cycle = communications[0][2]
  for index_idx in range(1, len(communications)):
    if not (shortest_repeating_cycle == 1 or communications[index_idx][2] == 1) and gcd(shortest_repeating_cycle, communications[index_idx][2]) == 1:
      raise ValueError("error@master_scheduler.py: {:4d} and {:4d} are relatively prime!".format(shortest_repeating_cycle, communications[index_idx][2]))
    if communications[index_idx][2] <= THRESHOLD_HYPER_PERIOD:
      print("warning@master_scheduler.py: for flow {" + "({:2d} -> {:2d})".format(communications[index_idx][0], communications[index_idx][1]) + "} the period (recommend: > " + "{:4d}, but found: ".format(THRESHOLD_HYPER_PERIOD) + "{:4d}".format(communications[index_idx][2]) + ") is maybe too small to work probably for a simulated network!\n")
    shortest_repeating_cycle = int(shortest_repeating_cycle * communications[index_idx][2] / gcd(shortest_repeating_cycle, communications[index_idx][2]))
  if shortest_repeating_cycle <= THRESHOLD_HYPER_PERIOD:
    print("warning@master_scheduler.py: the hyper period (recommend: > {:4d}, but computed: {:4d}) is maybe too small to work probably for a simulated network!\n".format(THRESHOLD_HYPER_PERIOD, shortest_repeating_cycle))
  flows = []

  flow_id = 1
  sub_flow_id = 1
  for communication in communications:
    source = communication[0]
    destination = communication[1]
    period = communication[2]
    if len(communication) > 3:
      release_time = communication[3]
      if release_time > 0:
        release_time -= 1
    else:
      release_time = 0
    if len(communication) > 4:
      deadline = communication[4]
    else:
      deadline = None
    # adding sub-flows based on the hyper-period
    for period_idx in range(0, shortest_repeating_cycle):
      if period_idx * period >= shortest_repeating_cycle:
        break
      flows.append(Flow(neighbor_parser.graph_etx, flow_id, sub_flow_id, source, destination, period, release_time + period_idx * period, deadline)) # , max_sub_flow_length)
      sub_flow_id += 1
    flow_id += 1
  schedule = Schedule(flows) # parser.graph_etx,
  schedule.create(etx_power, num_channels, shortest_repeating_cycle, scheduling_algorithm, scheduling_ignore_NSF, scheduling_strategy, scheduling_window_size_alg, fixed_window_size)

  if (num_cooja_nodes * 2 - 1) > schedule.available_slots:
    raise ValueError("There is not enough slots for Neighbor Data Collection Protocol")
      
  if args.print_schedule:
    print(schedule)

    # flow_path = []
    # config_schedule_algorithm_conf = open("/home/ptm/Documents/Design and Evaluation of Deadline-Based Scheduling Algorithms for the Industrial Internet of Things/software/MASTER/schedule/evaluation/software/config/config_" + str(args.algorithm) + ".conf", 'w')
    # copy_schedule = copy.copy(schedule)
    # copy_schedule.flows.sort(key = lambda x: x.flow_number)
    # config_schedule_algorithm_conf.write("# -----------------------------------------------------------------------------------------\n")
    # for flow in copy_schedule.flows:
    #   if not flow.output_flow_path() in flow_path:
    #     flow_path.append(flow.output_flow_path())
    #     flow_id -= 1
    #     config_schedule_algorithm_conf.write(flow.output_flow_path())
    #     if not flow_id - 1 == 0:
    #       config_schedule_algorithm_conf.write("\n")
    # config_schedule_algorithm_conf.write("\n# -----------------------------------------------------------------------------------------")
    # config_schedule_algorithm_conf.close()

  if generate_contiki_schedule:
    contiki_schedule = Contiki_schedule(neighbor_parser.graph_etx, schedule, node_ids, network_time_source)
    contiki_schedule.generate(contiki_output_file, schedule.flows, (contiki_minimal_schedule_length, 21) [contiki_minimal_schedule_length < 21], contiki_schedule_timesource)


if __name__ == "__main__":
  try:
    main()
  except Exception as failure:
    print(failure)
