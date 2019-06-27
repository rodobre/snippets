import subprocess
from multiprocessing import Process

# {(state, leter) -> state}
state_map = {}

# state list
states = set()
alphabet = set()

# reachable states
reachable_states = set()

# indistinguishable states
indistinguishable_states = []

# label states
state_label = {}
rstate_label = {}
minimized_initial_state = None
minimized_finals = set()
minimized_states = set()
minimized_map = {}
minimized_sts_map = {}
minimized_alphabet = set()

def hopcroft_minimization():
	partition = [final_states, states - final_states]
	W = [final_states]

	while len(W) != 0:
		set_A = W.pop()

		for l in alphabet:
			X = set()

			for s in states:
				if (s, l) in state_map:
					if state_map[(s, l)] in set_A:
						X.add(s)

			for Y in partition:
				if len(X & Y) == 0:
					continue

				if len(Y - X) == 0:
					continue

				intersect = X & Y
				diff = Y - X

				pos_in_P = partition.index(Y)
				partition[pos_in_P:pos_in_P + 1] = intersect, diff

				if Y in W:
					pos_in_W = W.index(Y)
					W[pos_in_W:pos_in_W + 1] = intersect, diff
				else:
					if len(intersect) < len(diff):
						W.append(intersect)
					else:
						W.append(diff)

	print("> Hopcroft minimization is finished")
	print partition
	return partition

def do_hopcroft_labelling(minimized_dfa):
	for miniset in minimized_dfa:
		miniset_idx = minimized_dfa.index(miniset) + 1
		for s in states:
			if s in miniset:
				if miniset_idx not in minimized_states:
					minimized_states.add(miniset_idx)

				if s == initial_state:
					global minimized_initial_state
					minimized_initial_state = miniset_idx

				if s in final_states:
					if minimized_dfa.index(miniset) + 1 not in minimized_finals:
						minimized_finals.add(miniset_idx)

				for l in alphabet:
					if (s, l) in state_map:
						minimized_alphabet.add(l)

				state_label[miniset_idx] = s
				rstate_label[s] = miniset_idx


def write_output(filename='output.txt'):
	with open(filename, 'w') as out:
		out.write(str(len(minimized_states)) + '\n')

		for s in minimized_states:
			out.write(str(s) + ' ')
		out.write('\n')

		out.write(str(len(minimized_alphabet)) + '\n')
		for l in minimized_alphabet:
			out.write(str(l) + ' ')
		out.write('\n')

		out.write(str(minimized_initial_state) + '\n')

		out.write(str(len(minimized_finals)) + '\n')
		for s in minimized_finals:
			out.write(str(s) + ' ')
		out.write('\n')

		minimized_transitions = set()

		for s in states:
			for l in alphabet:
				if (s, l) in state_map:
					lhs = rstate_label[s]
					rhs = rstate_label[state_map[(s, l)]]

					minimized_transitions.add((lhs, l, rhs))

		out.write(str(len(minimized_transitions)) + '\n')
		for pair in minimized_transitions:
			out.write(str(pair[0]) + ' ' + str(pair[1]) + ' ' + str(pair[2]) + '\n')

		# dummy words
		out.write('1\na\n')
		out.close()




def find_unreachable_states(current_state):
	adjacent_states = set()

	for l in alphabet:
		if (current_state, l) in state_map:
			if(state_map[(current_state, l)] not in reachable_states) and current_state not in reachable_states:
				adjacent_states.add(state_map[(current_state, l)])

	reachable_states.add(current_state)

	for state in adjacent_states:
		if state not in reachable_states:
			find_unreachable_states(state)

def remove_unreachable_states():
	for s in states:
		if s not in reachable_states:
			for l in alphabet:
				if (s, l) in state_map:
					del state_map[(s, l)]

def spawn_illustrators(file):
	subprocess.check_output('python illustrator.py ' + file, shell=True)

if __name__ == '__main__':
	state_count = 0
	letter_count = 0
	initial_state = 0
	final_state_count = 0
	final_states = set()
	transition_count = 0
	word_count = 0
	words = []

	with open('input.txt', 'r') as file:
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
			state_map[(int(tmp_transition[0]), tmp_transition[1])] = int(tmp_transition[2])

		word_count = int(file.readline().strip())

		for i in range(word_count):
			words.append(file.readline().strip())

	# strip unreachable states (if any) (bad)
	print "Old states: ", states
	find_unreachable_states(initial_state)
	remove_unreachable_states()
	print "New states: ", reachable_states

	print("Initial state: ", initial_state)
	print("Final states", final_states)

	states = reachable_states
	final_states = final_states & reachable_states

	minimized_dfa = hopcroft_minimization()
	do_hopcroft_labelling(minimized_dfa)
	write_output()

	execute_cmd = ["input.txt", "output.txt"]

	p1 = Process(target=spawn_illustrators, args=[execute_cmd[0]])
	p1.start()

	p2 = Process(target=spawn_illustrators, args=[execute_cmd[1]])
	p2.start()

	p1.join()
	p2.join()
