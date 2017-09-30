let stage;
let canvas;
let population = 10;
let target_count = 10;
let graph_width = 50;
let graph_height = 50;

let graph = new Array();

let units = new Array();

let targets = new Array();

class GraphNode {
    constructor(x, y) {
        this.x = x;
        this.y = y;

        this.shape = new createjs.Shape()
        this.shape.graphics.beginFill("white").drawCircle(0, 0, 1);

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
            graph[x].push(new GraphNode((x_scale / 2) + (x * x_scale), (y_scale / 2) + (y * y_scale)));
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
