// metrics
astar_calls = 0;
nodes_evaluated = 0;

class Vector {
    constructor(x, y) {
        this.x = x;
        this.y = y;
    }
}

class GraphNode {
    constructor(x, y, x_index, y_index) {
        this.x = x;
        this.y = y;

        this.x_index = x_index;
        this.y_index = y_index;

        this.obstacle = false;

        this.id = this.x_index + ':' + this.y_index;
    }

    equals(graph_node) {
        return (this.x == graph_node.x && this.y == graph_node.y);
    }
}

function magnitude(v) {
    let abs = Math.pow(v.x, 2) + Math.pow(v.y, 2);
    if (abs === 0) {
        return 0;
    }
    return Math.sqrt(abs);
}

function multiplyVector(v, x) {
    return new Vector(v.x * x, v.y * x)
}

function pythagorean(x1, y1, x2, y2) {
    let x_distance = x2 - x1;
    let y_distance = y2 - y1;
    return Math.sqrt(Math.pow(x_distance, 2) + Math.pow(y_distance, 2));
}

function vectorPythagorean(v1, v2) {
    return pythagorean(v1.x, v1.y, v2.x, v2.y);
}

function normalize(v) {
    let abs = magnitude(v);
    if (abs === 0) {
        return v;
    }
    return new Vector(v.x / abs, v.y / abs);
}

function getRandomInt(min, max) {
    min = Math.ceil(min);
    max = Math.floor(max);
    return Math.floor(Math.random() * (max - min)) + min;
}

function clamp(x, min, max) {
    return Math.max(Math.min(x, max), min);
}

function get(obj, key, fallback) {
    result = obj[key];
    if (result) {
        return result;
    }
    return fallback;
}

function aStar(start_node, goal_node, graph) {
    astar_calls++;

    if (start_node.equals(goal_node)) {
        return [];
    }

    let f_score = {};
    let g_score = {};
    let came_from = {};
    let open_set = new Set();
    let closed_set = new Set();

    open_set.add(start_node);

    g_score[start_node.id] = 0;

    f_score[start_node.id] = heuristicCostEstimate(start_node, goal_node);

    while (open_set.size > 0) {
        nodes_evaluated++;
        let current_node = null;
        let lowest_f_score = Number.POSITIVE_INFINITY;
        open_set.forEach((node) => {
            let node_f_score = f_score[node.id];
            if (node_f_score < lowest_f_score) {
                current_node = node;
                lowest_f_score = node_f_score;
            }
        })
        if (current_node.equals(goal_node)) {
            return reconstructPath(goal_node);
        }

        open_set.delete(current_node);
        closed_set.add(current_node);

        getNeighbors(current_node).forEach((neighbor) => {
            if (closed_set.has(neighbor)) {
                return;
            }
            if (!open_set.has(neighbor)) {
                open_set.add(neighbor);
            }
            let tentative_g_score = g_score[current_node.id] + distanceBetween(current_node, neighbor);
            if (tentative_g_score >= get(g_score, neighbor.id, Number.POSITIVE_INFINITY)) {
                return;
            }
            came_from[neighbor.id] = current_node;
            g_score[neighbor.id] = tentative_g_score;
            f_score[neighbor.id] = tentative_g_score + heuristicCostEstimate(neighbor, goal_node);
        });
    }
    return [];

    function reconstructPath(node) {
        let path = [];
        let current_node = node;
        path.push(current_node);
        while (get(came_from, current_node.id, null)) {
            current_node = came_from[current_node.id];
            path.unshift(current_node);
        }
        console.log("METRICS: A* calls: " + astar_calls)
        console.log("METRICS: Nodes evaluated: " + nodes_evaluated)
        return path;
    }

    function distanceBetween(node1, node2) {
        return pythagorean(node1.x, node1.y, node2.x, node2.y);

    }

    function heuristicCostEstimate(node1, node2) {
        return pythagorean(node1.x, node1.y, node2.x, node2.y);
    }

    function getNeighbors(node) {
        let neighbors = [];
        let graph_width = graph.length;
        let graph_height = graph[0].length;
        for (let x_shift = -1; x_shift <= 1; x_shift++) {
            for (let y_shift = -1; y_shift <= 1; y_shift++) {
                if (node.x_index + x_shift < 0 || node.x_index + x_shift >= graph_width) {
                    continue;
                }
                if (node.y_index + y_shift < 0 || node.y_index + y_shift >= graph_height) {
                    continue;
                }
                if (x_shift == 0 && y_shift == 0) {
                    continue;
                }
                let neighbor = graph[node.x_index + x_shift][node.y_index + y_shift];
                if (!neighbor.obstacle) {
                    neighbors.push(neighbor);
                }
            }
        }
        return neighbors;
    }
}

