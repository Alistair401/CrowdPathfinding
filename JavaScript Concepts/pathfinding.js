// 0 - A*
// 1 - Vector Field
let mode = 0;
let debug = false;

let stage;
let canvas;
let population = 100;
let target_count = 10;
let graph_width = 50;
let graph_height = 50;

let graph = new Array();

let units = new Array();

let targets = new Array();

let debug_shapes = new Array();

class GraphNode {
    constructor(x, y, x_index, y_index) {
        this.x = x;
        this.y = y;

        this.x_index = x_index;
        this.y_index = y_index;

        this.obstacle = false;
        this.shape = null;


        if (mode == 0) {
            this.g_score = Math.POSITIVE_INFINITY;
            this.came_from = null;
            this.f_score = Math.POSITIVE_INFINITY;
        }

        if (mode == 1) {
            this.vectors = new Array(target_count);
            this.target_distances = new Array(target_count);
        }
    }

    click() {
        this.obstacle = true;

        let x_scale = canvas.width / graph_width;
        let y_scale = canvas.height / graph_height;

        this.shape = new createjs.Shape()
        this.shape.graphics.beginFill("white").rect(-x_scale / 2, -y_scale / 2, x_scale, y_scale);

        this.shape.x = this.x;
        this.shape.y = this.y;

        stage.addChild(this.shape);
    }
}

class Unit {
    constructor(x, y) {
        this.x = x;
        this.y = y;

        this.target = null;

        this.path = null;

        this.shape = new createjs.Shape()
        this.shape.graphics.beginFill("red").drawCircle(0, 0, 8);

        this.shape.x = this.x;
        this.shape.y = this.y;

        stage.addChild(this.shape);
    }

    update() {
        if (!this.target) {
            let target_index = getRandomInt(0, target_count);
            this.target = targets[target_index];
        }
        if (mode == 0) {
            if (!this.path) {
                this.path = aStarPathfind(this.x, this.y, this.target.x, this.target.y);
            }
            if (this.path.length > 0) {
                let x_dist = this.path[this.path.length - 1].x - this.x;
                let y_dist = this.path[this.path.length - 1].y - this.y;
                let direction_vector = normalize(x_dist, y_dist);
                this.updatePosition(this.x + direction_vector.x, this.y + direction_vector.y)
                if (pythagorean(this.x, this.y, this.path[this.path.length - 1].x, this.path[this.path.length - 1].y) < 5) {
                    this.path.pop();
                }
            }
        }

        if (mode == 1) {
            let current_node = pointToGraph(this.x, this.y);
            let direction_vector = current_node.vectors[this.target.index];
            this.updatePosition(this.x + direction_vector.x, this.y + direction_vector.y)
        }
    }

    updatePosition(x, y) {
        this.x = x;
        this.y = y;
        this.shape.x = x;
        this.shape.y = y;
    }
}

class Target {
    constructor(x, y) {
        this.index = Target.count++;
        this.x = x;
        this.y = y;
        this.shape = new createjs.Shape()
        this.shape.graphics.beginFill("yellow").drawPolyStar(0, 0, 8, 5, 0.6, -90);;

        this.shape.x = this.x;
        this.shape.y = this.y;

        stage.addChild(this.shape);
    }
}
Target.count = 0;

class Vector {
    constructor(x, y) {
        this.x = x;
        this.y = y;
    }
}

$(document).ready(function () {
    initCanvas();
    initGraph();
    initUnits();
    initTargets();

    createjs.Ticker.addEventListener("tick", handleTick);

    $(canvas).mousedown((e) => {
        pointToGraph(e.pageX, e.pageY).click();

        if (mode == 1) {
            if (debug) {
                debug_shapes.forEach((shape) => {
                    stage.removeChild(shape)
                })
            }
            targets.forEach((target) => {
                vectorFieldWavefront(target.x, target.y, target.index);
            })
        }
        if (mode == 0) {
            units.forEach((unit) => {
                unit.path = aStarPathfind(unit.x, unit.y, unit.target.x, unit.target.y)
            })
        }

    })
});

function initCanvas() {
    canvas = $("#display")[0];
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;

    stage = new createjs.Stage("display");
}

function handleTick() {
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;

    units.forEach((unit) => {
        unit.update();
    })

    stage.update();
}

function initUnits() {
    for (let i = 0; i < population; i++) {
        let rand_x = Math.random() * canvas.width;
        let rand_y = Math.random() * canvas.height;
        units.push(new Unit(rand_x, rand_y));
    }
}

function initGraph() {
    let x_scale = canvas.width / graph_width;
    let y_scale = canvas.height / graph_height;
    for (let x = 0; x < graph_width; x++) {
        graph.push(new Array())
        for (let y = 0; y < graph_height; y++) {
            graph[x].push(new GraphNode((x_scale / 2) + (x * x_scale), (y_scale / 2) + (y * y_scale), x, y));
        }
    }
}

function pointToGraph(x, y) {
    let x_scale = canvas.width / graph_width;
    let y_scale = canvas.height / graph_height;

    let x_val = Math.round(((x - (x_scale / 2)) / canvas.width) * graph_width);
    let y_val = Math.round(((y - (y_scale / 2)) / canvas.height) * graph_height);
    return graph[x_val][y_val];
}

function initTargets() {
    for (let i = 0; i < target_count; i++) {
        let rand_x = Math.random() * canvas.width;
        let rand_y = Math.random() * canvas.height;
        new_target = new Target(rand_x, rand_y)
        targets.push(new_target);
        if (mode == 1) {
            vectorFieldWavefront(new_target.x, new_target.y, new_target.index);
        }
    }
}

function getRandomInt(min, max) {
    min = Math.ceil(min);
    max = Math.floor(max);
    return Math.floor(Math.random() * (max - min)) + min;
}

function normalize(x, y) {
    let abs = Math.pow(x, 2) + Math.pow(y, 2);
    abs = Math.sqrt(abs);
    return {
        x: x / abs,
        y: y / abs
    }
}

function aStarPathfind(x1, y1, x2, y2) {
    graph.forEach((column) => {
        column.forEach((node) => {
            node.came_from = null;
            node.f_score = Math.POSITIVE_INFINITY;
            node.g_score = Math.POSITIVE_INFINITY;
        });
    });

    let start_node = pointToGraph(x1, y1);
    let end_node = pointToGraph(x2, y2);

    start_node.g_score = 0;
    start_node.f_score = heuristicCostEstimate(start_node, end_node);


    let closed = new Array();

    let open = new Array();
    open.push(start_node);

    while (open.length > 0) {
        let current;
        let lowest_index = 0;
        for (let i = 0; i < open.length; i++) {
            if (open[i].f_score < open[lowest_index].f_score) {
                lowest_index = i;
            }
        }
        current = open[lowest_index];
        if (current == end_node) {

            let path = new Array();
            path.push(current);
            while (current.came_from) {
                current = current.came_from;
                path.push(current);
            }
            return path;
        }
        open.splice(lowest_index, 1);
        closed.push(current);

        getNeighbors(current).forEach((neighbor) => {
            if (closed.indexOf(neighbor) != -1) {
                return;
            }

            if (open.indexOf(neighbor) == -1) {
                open.push(neighbor);
            }

            tentative_g_score = current.g_score + heuristicCostEstimate(current, neighbor);
            if (tentative_g_score > neighbor.g_score) {
                return;
            }

            neighbor.came_from = current;
            neighbor.g_score = tentative_g_score;
            neighbor.f_score = neighbor.g_score + heuristicCostEstimate(neighbor, end_node);
        });
    }

    return new Array();

    function heuristicCostEstimate(node1, node2) {
        return pythagorean(node1.x, node1.y, node2.x, node2.y);
    }
}

function vectorFieldWavefront(target_x, target_y, target_index) {
    let target_node = pointToGraph(target_x, target_y);

    let open = new Array();
    let closed = new Array();

    target_node.target_distances[target_index] = 0;

    open.push(target_node);
    closed.push(target_node);

    while (open.length > 0) {
        let current_node = open[0];
        open.splice(0, 1);

        getAdjacent(current_node).forEach((neighbor) => {
            if (closed.indexOf(neighbor) != -1) {
                return;
            }
            neighbor.target_distances[target_index] = current_node.target_distances[target_index] + 1;

            closed.push(neighbor);
            open.push(neighbor);
        });
    }

    graph.forEach((column) => {
        column.forEach((node) => {
            let neighbors = getNeighbors(node);
            let nearest_neighbor = neighbors[0];
            neighbors.forEach((neighbor) => {
                if (neighbor.target_distances[target_index] < nearest_neighbor.target_distances[target_index]) {
                    nearest_neighbor = neighbor;
                }
            });
            let x_dist = nearest_neighbor.x - node.x;
            let y_dist = nearest_neighbor.y - node.y;
            let direction_vector;
            if (node.target_distances[target_index] == 0) {
                direction_vector = new Vector(0, 0)
            } else {
                direction_vector = normalize(x_dist, y_dist);
            }
            node.vectors[target_index] = new Vector(direction_vector.x, direction_vector.y);

            if (debug) {
                let line = new createjs.Shape();
                line.graphics.setStrokeStyle(1);
                line.graphics.beginStroke("white");
                line.graphics.moveTo(node.x, node.y);
                line.graphics.lineTo(node.x + (direction_vector.x * 6), node.y + (direction_vector.y) * 6);
                line.graphics.endStroke();

                stage.addChild(line)
                debug_shapes.push(line)

                //                var text = new createjs.Text(node.target_distances[target_index], "12px Arial", "white");
                //                text.x = node.x;
                //                text.y = node.y;
                //                stage.addChild(text)
                //                debug_shapes.push(text)
            }

        });
    });

}

function getNeighbors(node) {
    let neighbors = new Array();
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
            neighbors.push(graph[node.x_index + x_shift][node.y_index + y_shift]);
        }
    }
    return neighbors.filter((node) => {
        return !node.obstacle;
    });
}

function getAdjacent(node) {
    let adjacent = new Array();
    for (let x_shift = -1; x_shift <= 1; x_shift++) {
        if (node.x_index + x_shift < 0 || node.x_index + x_shift >= graph_width) {
            continue;
        }
        if (x_shift == 0) {
            continue;
        }
        adjacent.push(graph[node.x_index + x_shift][node.y_index]);
    }
    for (let y_shift = -1; y_shift <= 1; y_shift++) {
        if (node.y_index + y_shift < 0 || node.y_index + y_shift >= graph_height) {
            continue;
        }
        if (y_shift == 0) {
            continue;
        }
        adjacent.push(graph[node.x_index][node.y_index + y_shift]);
    }
    return adjacent.filter((node) => {
        return !node.obstacle;
    });
}

function pythagorean(x1, y1, x2, y2) {
    let x_distance = x2 - x1;
    let y_distance = y2 - y1;
    return Math.sqrt(Math.pow(x_distance, 2) + Math.pow(y_distance, 2));
}
