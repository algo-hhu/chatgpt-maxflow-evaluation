import networkx as nx
import matplotlib.pyplot as plt
import random

def validate_parameters(num_nodes, num_edges, source_conn, sink_conn):
    if num_nodes < 2:
        raise ValueError("A flow network requires at least two nodes.")

    if source_conn < 1 or sink_conn < 1:
        raise ValueError("The source and sink must be connected to at least one node.")

    if source_conn + sink_conn > num_nodes - 2:
        raise ValueError("Too many source or sink connections given the number of intermediate nodes.")

    min_edges_required = source_conn + sink_conn
    if num_edges < min_edges_required:
        raise ValueError(f"Number of edges ({num_edges}) is too small. It must be at least {min_edges_required}.")

def generate_random_flow_network(num_nodes, num_edges, max_capacity, source_conn, sink_conn):
    """
    Generate a random flow network with specific characteristics.

    Parameters:
        num_nodes (int): Total number of nodes (including source and sink).
        num_edges (int): Total number of edges in the network.
        max_capacity (int): Maximum capacity for an edge.
        source_conn (int): Number of nodes the source is connected to.
        sink_conn (int): Number of nodes that connect to the sink.

    Returns:
        nx.DiGraph, int, int: The generated network, source node, and sink node.
    """
    validate_parameters(num_nodes, num_edges, source_conn, sink_conn)

    # Create a directed graph
    G = nx.DiGraph()

    # Add nodes
    G.add_nodes_from(range(num_nodes))

    # Set source and sink
    source = 0
    sink = num_nodes - 1

    # Intermediate nodes
    intermediate_nodes = list(range(1, num_nodes - 1))

    # Connect the source to a subset of intermediate nodes
    source_nodes = random.sample(intermediate_nodes, source_conn)
    for target in source_nodes:
        capacity = random.randint(1, max_capacity)
        G.add_edge(source, target, capacity=capacity)

    # Connect a subset of intermediate nodes to the sink
    sink_nodes = random.sample(intermediate_nodes, sink_conn)
    for source_node in sink_nodes:
        capacity = random.randint(1, max_capacity)
        G.add_edge(source_node, sink, capacity=capacity)

    # Add remaining random edges between intermediate nodes
    remaining_edges = num_edges - G.number_of_edges()
    existing_edges = set(G.edges())
    while remaining_edges > 0:
        u, v = random.sample(intermediate_nodes, 2)
        if (u, v) not in existing_edges:
            capacity = random.randint(1, max_capacity)
            G.add_edge(u, v, capacity=capacity)
            existing_edges.add((u, v))
            remaining_edges -= 1

    # Relabel nodes: source is 1, sink is 2
    mapping = {source: 1, sink: 2}
    for node in range(1, num_nodes - 1):
        mapping[node] = node + 2

    G = nx.relabel_nodes(G, mapping)

    return G, 1, 2

def draw_flow_network(G):
    """
    Draws the flow network using NetworkX and Matplotlib.

    Parameters:
        G (nx.DiGraph): The flow network to be drawn.
    """
    pos = nx.spring_layout(G)
    edge_labels = {(u, v): d['capacity'] for u, v, d in G.edges(data=True)}

    plt.figure(figsize=(10, 8))
    nx.draw(G, pos, with_labels=True, node_size=700, node_color='lightblue', arrows=True)
    nx.draw_networkx_edge_labels(G, pos, edge_labels=edge_labels, label_pos=0.3)
    plt.title("Flow Network")
    plt.show()

def generate_dimacs_graph(G, source, sink, file_path):
    """
    Generates a graph in DIMACS format and includes the maximum flow value.

    Parameters:
        G (nx.DiGraph): The flow network.
        source (int): The source node.
        sink (int): The sink node.
        file_path (str): The path where the .max file will be saved.
    """
    # Compute maximum flow
    flow_value, flow_dict = nx.maximum_flow(G, source, sink)
    
    with open(file_path, 'w') as f:
        f.write("c Example DIMACS file for a max flow problem\n")
        f.write(f"c Maximum flow: {flow_value}\n")
        f.write(f"p max {G.number_of_nodes()} {G.number_of_edges()}\n")
        f.write(f"n {source} s\n")
        f.write(f"n {sink} t\n")
        for u, v, data in G.edges(data=True):
            capacity = data['capacity']
            f.write(f"a {u} {v} {capacity}\n")

def generate_multiple_flow_networks(num_graphs, num_nodes, num_edges, max_capacity, source_conn, sink_conn, file_prefix, start_suffix, end_suffix):
    """
    Generates multiple flow networks and saves them in DIMACS format files with specified suffix range.

    Parameters:
        num_graphs (int): Number of graphs to generate.
        num_nodes (int): Total number of nodes (including source and sink).
        num_edges (int): Total number of edges in the network.
        max_capacity (int): Maximum capacity for an edge.
        source_conn (int): Number of nodes the source is connected to.
        sink_conn (int): Number of nodes that connect to the sink.
        file_prefix (str): Prefix for output .max files.
        start_suffix (int): Starting suffix for the output files.
        end_suffix (int): Ending suffix for the output files.
    """
    if end_suffix - start_suffix + 1 < num_graphs:
        raise ValueError("The range of suffixes is smaller than the number of graphs to generate.")
    
    suffixes = range(start_suffix, end_suffix + 1)
    
    for i in range(num_graphs):
        G, source, sink = generate_random_flow_network(num_nodes, num_edges, max_capacity, source_conn, sink_conn)
        file_path = f"{file_prefix}_{suffixes[i]}.max"
        generate_dimacs_graph(G, source, sink, file_path)
        print(f"Generated {file_path}")

# Example usage
if __name__ == "__main__":
    num_graphs = 1  
    num_nodes = 4000000  
    num_edges = 22000000  
    max_capacity = 242214891
    source_conn = max(1, num_nodes // 17)  
    sink_conn = max(1, num_nodes // 15)  
    file_prefix = 'flow_network'
    start_suffix = 17  # Starting suffix for the output file(s)
    end_suffix = 17   # Ending suffix for the output file(s)

    generate_multiple_flow_networks(num_graphs, num_nodes, num_edges, max_capacity, source_conn, sink_conn, file_prefix, start_suffix, end_suffix)

