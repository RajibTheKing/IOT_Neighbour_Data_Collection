#!/usr/local/bin/python3
import os
from re import search as re_search
from os.path import dirname, abspath
from enum import Enum

class Data_table(Enum):
  etx = 0
  prr = 1
  rssi = 2
  strength_vector_prr = 3
  strength_vector_etx = 4

class Parser(object):

  def __init__(self, valid_node_ids, folder_path, filename=None, max_etx_bound=None, min_rssi_bound=None):
    self.valid_node_ids = valid_node_ids
    if folder_path[-1] == '/':
      self.folder = folder_path
    else:
      self.folder = folder_path + '/'
    self.filename = filename
    self.max_etx_bound = max_etx_bound
    self.min_rssi_bound = min_rssi_bound
    self.parse_graphs = []
    self.strength_vector_prr = {}
    self.strength_vector_etx = {}

  def add_to_parse_graph(self, number, sender, receiver=0, rssi=0):
    if sender not in self.parse_graphs[-1]:
      self.parse_graphs[-1][sender] = {}
    if receiver:
        if receiver not in self.parse_graphs[-1][sender]:
            self.parse_graphs[-1][sender][receiver] = []
        self.parse_graphs[-1][sender][receiver].append((number, rssi))
    else:
        if sender not in self.parse_graphs[-1][sender]:
            self.parse_graphs[-1][sender][sender] = []
        self.parse_graphs[-1][sender][sender].append(number)

  def match_neighbor_data(self, line):
    match = re_search(r'rcvd;(\d+);(\d+);(\d+);(\d+);(-?\d+)', line)                      # rcvd;<node_id>;<sender>;<channel>;<number>;<rssi>
    if match:
      packet_number = int(match.group(4))
      sender        = int(match.group(2))
      receiver      = int(match.group(1)) # == node_id
      rssi          = int(match.group(5))
      # channel     = int(match.group(3)) # currently unused, but can be used for channel statistics

      if receiver in self.valid_node_ids and sender in self.valid_node_ids:
        self.add_to_parse_graph(packet_number, sender, receiver, rssi)

    else:
      match = re_search(r'sent;(\d+);(\d+)', line)                                        # sent;<node_id>;<number>
      if match:
        packet_number = int(match.group(2))
        sender        = int(match.group(1))

        if sender in self.valid_node_ids:
          self.add_to_parse_graph(packet_number, sender)


  def match_strength_vector(self, line):
    #strength_vector(60): (1 - .633), (2 - .500), (3 - .850), (4 - .516), (5 - .000), 
    #regex = 'strength_vector\(\d+\):( \(\d+ \- (\d+)*\.\d+\),)*'
    # match = re_search(r'strength_vector\(\d+\):( \(\d+ \- (\d+)*\.\d+\),)*', line) 
    match = line if ('strength_vector' in line) else None 
    if match:
      #print('match', match)
      space_split_data = match.split(' ')
      rcv_node_id = int (space_split_data[1])
      strength_data = space_split_data[2].split(',')

      for x in strength_data:
        y = x.split('-')
        if len(y) > 1 and rcv_node_id != int (y[0]):
          if (int(y[0]) in self.strength_vector_prr.keys()) and (int(y[0]) in self.strength_vector_etx.keys()): 
            pass
          else:
            self.strength_vector_prr[int (y[0])] = {}
            self.strength_vector_etx[int (y[0])] = {}

          self.strength_vector_prr[int (y[0])][rcv_node_id] = float (y[1])
          self.strength_vector_etx[int (y[0])][rcv_node_id]  = 1 /float (y[1])



  def parse_file(self, filepath):
    print(filepath)
    with open(filepath, encoding="utf8", errors='ignore') as f:
      self.parse_graphs.append({})
      for line in f:
        self.match_neighbor_data(line)
        self.match_strength_vector(line)
      #print('strength_vector_prr\n',self.strength_vector_prr)
      #self.print_parsed_data_table(Data_table.strength_vector_prr)
      #elf.print_parsed_data_table(Data_table.strength_vector_etx)


  def combine_parse_graphs(self):
    self.graph_etx = {}
    self.graph_prr = {}
    self.graph_rssi = {}

    for sender in self.valid_node_ids:
      self.graph_etx[sender] = {}
      self.graph_prr[sender] = {}
      self.graph_rssi[sender] = {}
      highest_received_number = []
      received_packets_count = []

      for parse_graph in self.parse_graphs:
        received_packets_count.append({})

        if sender in [*parse_graph]:
          highest_received_number.append(0)
          for receiver in [*parse_graph[sender]]:
            if receiver != sender:
              last_received_packet_number = parse_graph[sender][receiver][-1][0]
              if last_received_packet_number > highest_received_number[-1]:
                highest_received_number[-1] = last_received_packet_number
              received_packets_count[-1][receiver] = len(parse_graph[sender][receiver])

      # if sender exists anywhere:
      if sum(highest_received_number):
        total_number_receivable_packets = float(sum(highest_received_number))
        for receiver in self.valid_node_ids:
          if receiver != sender:
            total_received_packets_count = 0
            sums_rssi = []
            for idx in range(len(received_packets_count)):
              if receiver in [*received_packets_count[idx]]:
                total_received_packets_count += received_packets_count[idx][receiver]

              # rssi calc
              if (sender in [*self.parse_graphs[idx]]) and (receiver in [*self.parse_graphs[idx][sender]]):
                sums_rssi.append(sum(rssi for _, rssi in self.parse_graphs[idx][sender][receiver])) # [(n, r), (n, r), ...]
            if total_received_packets_count > 0:
              prr = float(total_received_packets_count) / total_number_receivable_packets
              etx = total_number_receivable_packets / float(total_received_packets_count)
              if (not self.max_etx_bound) or (etx < self.max_etx_bound):
                self.graph_prr[sender][receiver] = prr
                self.graph_etx[sender][receiver] = etx

              # rssi calc
              avg_rssi_value = float(sum(sums_rssi)) / float(total_received_packets_count)
              if (not self.min_rssi_bound) or (avg_rssi_value > self.min_rssi_bound):
                self.graph_rssi[sender][receiver] = avg_rssi_value
    # for sender in self.valid_node_ids:
    #   for receiver in self.valid_node_ids:
    #     # print(sender, receiver)
    #     # print(self.graph_etx[receiver])
    #     if sender != receiver and (sender in self.graph_etx[receiver] or receiver in self.graph_etx[sender]):
    #       common_etx = 0.0
    #       common_prr = 0.0
    #       common_rssi = 0.0
    #       num_occurences = 0
    #       if sender in self.graph_etx[receiver]:
    #         common_etx += self.graph_etx[receiver][sender]
    #         common_prr += self.graph_prr[receiver][sender]
    #         common_rssi += self.graph_rssi[receiver][sender]
    #         num_occurences += 1
    #       if receiver in self.graph_etx[sender]:
    #         common_etx += self.graph_etx[sender][receiver]
    #         common_prr += self.graph_prr[sender][receiver]
    #         common_rssi += self.graph_rssi[sender][receiver]
    #         num_occurences += 1
    #       self.graph_etx[sender][receiver] = common_etx / num_occurences
    #       self.graph_etx[receiver][sender] = common_etx / num_occurences
    #       self.graph_prr[sender][receiver] = common_prr / num_occurences
    #       self.graph_prr[receiver][sender] = common_prr / num_occurences
    #       self.graph_rssi[sender][receiver] = common_rssi / num_occurences
    #       self.graph_rssi[receiver][sender] = common_rssi / num_occurences

  def print_parsed_data_table(self, table_type):
    if table_type == Data_table.etx:
      output = 'ETX'
      graph = self.graph_etx
      entry_format_string = '{:8.3f}'
    elif table_type == Data_table.prr:
      output = 'PRR'
      graph = self.graph_prr
      entry_format_string = '{:8.3f}'
    elif table_type == Data_table.rssi:
      output = 'RSSI'
      graph = self.graph_rssi
      entry_format_string = '{:8.1f}'
    elif table_type == Data_table.strength_vector_prr:
      output = 'Strength Vector PRR'
      graph = self.strength_vector_prr
      entry_format_string = '{:8.3f}'
    elif table_type == Data_table.strength_vector_etx:
      output = 'Strength Vector ETX'
      graph = self.strength_vector_etx
      entry_format_string = '{:8.3f}'

    output += ' table:\n^^^^^^^^^^\nr\\f'
    for sender in self.valid_node_ids:
      output += '{:8d}'.format(sender)
    output += '\n'
    for receiver in self.valid_node_ids:
      output += '{:3d}'.format(receiver)
      for sender in self.valid_node_ids:
        if sender in graph:
          receiver_ids = [*graph[sender]]
          if receiver in receiver_ids:
            output += entry_format_string.format(graph[sender][receiver])
          else:
            output += '        '
      output += '\n'
    print(output)


  def parse_neighbor_data(self, print_etx=False, print_prr=False, print_rssi=False, print_strength_vector=False):
    if self.filename:
      self.parse_file(self.folder + self.filename)
    else:
      for filename in os.listdir(self.folder):
        self.parse_file(self.folder + filename)
    self.combine_parse_graphs()
    if print_prr:
      self.print_parsed_data_table(Data_table.prr)
    if print_etx:
      self.print_parsed_data_table(Data_table.etx)
    if print_rssi:
      self.print_parsed_data_table(Data_table.rssi)
    if print_strength_vector:
      self.print_parsed_data_table(Data_table.strength_vector_prr)
      self.print_parsed_data_table(Data_table.strength_vector_etx)
      self.graph_prr = self.strength_vector_prr
      self.graph_etx = self.strength_vector_etx

  def create_DGRM_configuration(self):
    dgrm_conf = open(str(dirname(dirname(abspath(__file__)))) + "/DGRM_configurations/dgrm.conf", 'w')
    dgrm_conf.write("# [source] [destination] [prr] [prr_ci] [num_tx] [num_rx] [rssi] [rssi_min] [rssi_max]\n")
    to_exclude = [11]
    index = len([*self.graph_prr])
    for sender in [*self.graph_prr]:
      if sender in to_exclude:
        continue
      index -= 1
      sindex = len([*self.graph_prr[sender]])
      for receiver in [*self.graph_prr[sender]]:
        sindex -= 1
        dgrm_conf.write("{} {} {:5.3f} 0 0 0 {:5.3f} 0 0".format(sender, receiver, self.graph_prr[sender][receiver], self.graph_rssi[sender][receiver]))
        if not sindex == 0:
          dgrm_conf.write("\n")
      if not index == 0:
        dgrm_conf.write("\n")
    dgrm_conf.close()