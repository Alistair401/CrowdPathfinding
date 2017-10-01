// 0 - A*
// 1 - Vector Field
let mode = 1;

let stage;
let canvas;
let population = 1000;
let target_count = 10;
let graph_width = 50;
let graph_height = 50;

let graph = new Array();

let units = new Array();

let targets = new Array();

class GraphNode {
    constructor(x, y, x_index, y_index) {
        this.x = x;
        this.y = y;

        this.x_index = x_index;
        this.y_index = y_index;

        this.g_score = Math.POSITIVE_INFINITY;
        this.came_from = null;
        this.f_score = Math.POSITIVE_INFINITY;
    }
}

class Unit {
    constructor(x, y) {
        this.x = x;
        this.y = y;

        this.x_vel = 0;
        this.y_vel = 0;

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
                this.x_vel = this.path[this.path.length - 1].x - this.x;
                this.y_vel = this.path[this.path.length - 1].y - this.y;
                let direction_vector = normalize(this.x_vel, this.y_vel);
                this.updatePosition(this.x + direction_vector.x, this.y + direction_vector.y)
                if (pythagorean(this.x, this.y, this.path[this.path.length - 1].x, this.path[this.path.length - 1].y) < 5) {
                    this.path.pop();
                }
            }
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
        this.x = x;
        this.y = y;
        this.shape = new createjs.Shape()
        this.shape.graphics.beginFill("yellow").drawPolyStar(0, 0, 8, 5, 0.6, -90);;

        this.shape.x = this.x;
        this.shape.y = this.y;

        stage.addChild(this.shape);

        if (mode == 1) {
            this.vector_field = vectorFieldWavefront(this.x, this.y);
        }
    }
}

class vectorFieldVector {
    constructor(x,y){
        
    }
}

$(document).ready(function () {
    initCanvas();
    initGraph();
    initUnits();
    initTargets();

    createjs.Ticker.addEventListener("tick", handleTick);
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
        targets.push(new Target(rand_x, rand_y));
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

function vectorFieldWavefront(target_x, target_y) {
    let target_node = pointToGraph(target_x,target_y);
    
    open = new Array();
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
    return neighbors;
}

function pythagorean(x1, y1, x2, y2) {
    let x_distance = x2 - x1;
    let y_distance = y2 - y1;
    return Math.sqrt(Math.pow(x_distance, 2) + Math.pow(y_distance, 2));
}
