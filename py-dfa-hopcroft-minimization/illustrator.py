import networkx as nx
import matplotlib.pyplot as plt
import edges as e
import sys

# {(state, state) -> letter}
state_to_state_transition = {}

# list of node colors
node_color_array = []

# declare G as a multiple-edge directional graph
G = nx.MultiDiGraph()

# colors for the DFA
initial_state_color = 'red'
final_state_color = 'green'
another_state_color = 'orange'

if __name__ == '__main__':
	arguments = sys.argv
	if (len(arguments) < 2):
		print("Invalid command, " + str(arguments[0]) + " <file>")
		exit(1)

	state_count = 0
	states = set()
	letter_count = 0
	alphabet = []
	initial_state = 0
	final_state_count = 0
	final_states = set()
	transition_count = 0
	word_count = 0
	words = []

	with open(arguments[1], 'r') as file:
		state_count = int(file.readline().strip())
		states = set(int(i) for i in file.readline().split())
		letter_count = int(file.readline().strip())
		alphabet = set(str(i) for i in file.readline().split())
		initial_state = int(file.readline().strip())
		final_state_count = int(file.readline().strip())
		final_states = set(int(i) for i in file.readline().split())
		transition_count = int(file.readline().strip())

		for i in range(transition_count):
			tmp_transition = file.readline().split()
			state_to_state_transition[(tmp_transition[0], tmp_transition[2])] = tmp_transition[1]

			G.add_edge(tmp_transition[0], tmp_transition[2])

		for node_str in G.nodes():
			node = int(node_str)

			if node == initial_state:
				node_color_array.append(initial_state_color)
			elif node in final_states:
				node_color_array.append(final_state_color)
			else:
				node_color_array.append(another_state_color)

	pos = nx.spring_layout(G)
	for edge in G.edges():
		e.draw_networkx_multi_edges(G, pos, edgelist=[edge], width=1.0, alpha=0.6, arrows=True, label=state_to_state_transition[edge], rad=0.3)
	nx.draw_networkx_nodes(G, pos, nodelist=G.nodes(), node_size=300, node_color=node_color_array, alpha=0.8)
	nx.draw_networkx_labels(G, pos, labels={node:node for node in G.nodes()})
	e.draw_networkx_edge_labels(G, pos, edge_labels=state_to_state_transition, label_pos=0.5, alpha=1.0, rotate=True)

	# pyplot render
	plt.axis('off')
	plt.show()

