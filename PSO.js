let population = 100;
let target_count = 3;
let global_weight = 1;
let personal_weight = 1;
let debug = true;

let stage;
let canvas;

let targets = new Array();

let boids = new Array();

let debug_shapes = new Array();

class Boid {
    constructor(x, y) {
        this.x = x;
        this.y = y;

        let target_index = getRandomInt(0, target_count);
        this.target = targets[target_index];

        this.velocity = normalize(new Vector(this.target.x - this.x, this.target.y - this.y));

        this.best_position = new Vector(x, y);
        this.best_fitness = Number.POSITIVE_INFINITY;
        this.fitness = Number.POSITIVE_INFINITY;

        this.shape = new createjs.Shape();
        this.shape.graphics.beginFill("red").drawCircle(0, 0, 8);

        this.shape.x = this.x;
        this.shape.y = this.y;

        stage.addChild(this.shape);
    }

    update() {
        // Lower fitness is better
        this.fitness = pythagorean(this.x, this.y, this.target.x, this.target.y);
        if (this.fitness < this.best_fitness) {
            this.best_fitness = this.fitness;
            this.best_position = new Vector(this.x, this.y);
        }
    }

    updateHeading(global_best) {
        this.velocity.x = this.velocity.x + ((global_best.x - this.x) * Math.random() * global_weight) + ((this.best_position.x - this.x) * Math.random() * personal_weight);
        this.velocity.y = this.velocity.y + ((global_best.y - this.y) * Math.random() * global_weight) + ((this.best_position.y - this.y) * Math.random() * personal_weight);
        this.velocity = normalize(this.velocity)
    }

    updatePosition() {
        this.x = this.x + this.velocity.x;
        this.y = this.y + this.velocity.y;
        this.shape.x = this.x;
        this.shape.y = this.y;
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

$(document).ready(() => {
    initCanvas();
    initTargets();
    initBoids();

    createjs.Ticker.addEventListener("tick", handleTick);
});

function initCanvas() {
    canvas = $("#display")[0];
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;

    boids.forEach((boid) => {
        boid.update();
    });

    stage = new createjs.Stage("display");
}

function initTargets() {
    for (let i = 0; i < target_count; i++) {
        let rand_x = Math.random() * canvas.width;
        let rand_y = Math.random() * canvas.height;
        new_target = new Target(rand_x, rand_y)
        targets.push(new_target);
    }
}

function initBoids() {
    for (let i = 0; i < population; i++) {
        let rand_x = Math.random() * canvas.width;
        let rand_y = Math.random() * canvas.height;
        boids.push(new Boid(rand_x, rand_y));
    }
}

function handleTick() {
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;

    boids.forEach((boid) => {
        boid.update();
    })
    boids.forEach((boid) => {
        boid.updateHeading(getBest());
        boid.updatePosition();
    })

    stage.update();

    debug_shapes.forEach((shape) => {
        stage.removeChild(shape)
    });
    debug_shapes = new Array();
}

// Unlikely but may return the ignored boid
function getBest(ignore) {
    let best = boids[0];

    boids.forEach((boid) => {
        if (boid != ignore && boid.fitness < best.fitness) {
            best = boid;
        }
    })

    return best;
}

function pythagorean(x1, y1, x2, y2) {
    let x_distance = x2 - x1;
    let y_distance = y2 - y1;
    return Math.sqrt(Math.pow(x_distance, 2) + Math.pow(y_distance, 2));
}

function normalize(v) {
    let abs = magnitude(v);
    if (abs == 0) {
        return v;
    }
    return new Vector(v.x / abs, v.y / abs);
}

function clamp(x, min, max) {
    return Math.max(Math.min(x, max), min);
}

function multiplyVector(v, x) {
    return new Vector(v.x * x, v.y * x)
}

// Returns a random value in [min,max)
function getRandomInt(min, max) {
    min = Math.ceil(min);
    max = Math.floor(max);
    return Math.floor(Math.random() * (max - min)) + min;
}

function magnitude(v) {
    let abs = Math.pow(v.x, 2) + Math.pow(v.y, 2);
    if (abs == 0) {
        return 0;
    }
    return Math.sqrt(abs);
}
