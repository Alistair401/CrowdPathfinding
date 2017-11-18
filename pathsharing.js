let stage;
let canvas;
let population = 100;
let target_count = 10;

let graph = [];
let graph_width = 50;
let graph_height = 50;

let units = [];

let targets = [];

let path_arr = [];

class Unit {
    constructor(x, y) {
        this.x = x;
        this.y = y;

        this.target = null;

        this.path = [];

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
        if (this.path.length == 0 && pythagorean(this.x, this.y, this.target.x, this.target.y > 50)) {
            // let possible_paths = path_arr.filter((p) => {
            //     let distance_to_start = pythagorean(this.x, this.y, p[0].x, p[0].y);
            //     let distance_to_target = pythagorean(this.x, this.y, p[p.length - 1].x, p[p.length - 1].y);
            //     let direct_distance = pythagorean(this.x, this.y, this.target.x, this.target.y)
            //     return distance_to_start + distance_to_target < direct_distance;
            // });
            // if (possible_paths.length == 0) {
            //     this.path = aStar(pointToGraph(this.x, this.y), pointToGraph(this.target.x, this.target.y), graph);
            //     if (this.path.length > 0) {
            //         path_arr.push(this.path.slice(0));
            //     }
            // } else {
            //     let lowest_distance = Number.POSITIVE_INFINITY;
            //     let closest_path = null;
            //     possible_paths.forEach((p) => {
            //         let distance_to_start = pythagorean(this.x, this.y, p[0].x, p[0].y);
            //         let distance_to_target = pythagorean(this.x, this.y, p[p.length - 1].x, p[p.length - 1].y);
            //         if (distance_to_start + distance_to_target < lowest_distance) {
            //             lowest_distance = distance_to_start + distance_to_target;
            //             closest_path = p;
            //         }
            //     });
            //     let chosen_path = closest_path;
            //     let path_start = aStar(pointToGraph(this.x, this.y), pointToGraph(chosen_path[0].x, chosen_path[0].y), graph);
            //     let path_end = aStar(pointToGraph(chosen_path[chosen_path.length - 1].x, chosen_path[chosen_path.length - 1].y), pointToGraph(this.target.x, this.target.y), graph);
            //     this.path = [...path_start, ...chosen_path, ...path_end];
            // }
            this.path = aStar(pointToGraph(this.x, this.y), pointToGraph(this.target.x, this.target.y), graph);
        }
        if (this.path.length > 0) {
            let x_dist = this.path[0].x - this.x;
            let y_dist = this.path[0].y - this.y;
            let direction_vector = normalize(new Vector(x_dist, y_dist));
            this.updatePosition(this.x + direction_vector.x, this.y + direction_vector.y)
            if (pythagorean(this.x, this.y, this.path[0].x, this.path[0].y) < 5) {
                this.path.shift();
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

$(document).ready(() => {
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

    console.log(path_arr.length)
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
    }
}
