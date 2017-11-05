let population = 100;
let boid_radius = 75;
let target_count = 10;
let cohesion_factor = 10;
let separation_factor = 10;
let alignment_factor = 10;
let target_factor = 15;
let debug = true;

let stage;
let canvas;

let targets = [];

let boids = [];

let debug_shapes = [];

class Boid {
    constructor(x, y) {
        this.x = x;
        this.y = y;

        let target_index = getRandomInt(0, target_count);
        this.target = targets[target_index];

        this.heading = normalize(new Vector(this.target.x - this.x, this.target.y - this.y));
        
        this.shape = new createjs.Shape();
        this.shape.graphics.beginFill("red").drawCircle(0, 0, 8);

        this.shape.x = this.x;
        this.shape.y = this.y;

        stage.addChild(this.shape);
    }

    update() {
        
        let target_distance = pythagorean(this.x,this.y,this.target.x,this.target.y);
        let target_vector = new Vector(this.target.x - this.x, this.target.y - this.y);
        target_vector = normalize(target_vector);
        target_vector = multiplyVector(target_vector, target_factor);

        // Get local flockmates
        // A) Steer to avoid local flockmates
        // B) Steer towards the average heading of local flockmates
        // C) Steer to move towards the average position of flockmates

        let flockmates = [];

        boids.forEach((boid) => {
            if (boid == this) {
                return;
            }
            if (pythagorean(this.x, this.y, boid.x, boid.y) < boid_radius) {
                flockmates.push(boid);
            }
        });

        if (flockmates.length == 0) {
            this.updatePosition(this.x + this.heading.x, this.y + this.heading.y);
            return;
        }

        // A
        let separation_vector = new Vector(0, 0);

        flockmates.forEach((flockmate) => {
            if (pythagorean(this.x, this.y, flockmate.x, flockmate.y) < boid_radius / 2) {
                separation_vector.x += flockmate.x - this.x;
                separation_vector.y += flockmate.y - this.y;
            }
        });

        separation_vector.x *= -1;
        separation_vector.y *= -1;
        separation_vector = normalize(separation_vector);
        separation_vector = multiplyVector(separation_vector, separation_factor);

        // B 
        let alignment_vector = new Vector(0, 0);

        flockmates.forEach((flockmate) => {
            alignment_vector.x += flockmate.heading.x;
            alignment_vector.y += flockmate.heading.y;
        });

        alignment_vector.x /= flockmates.length;
        alignment_vector.y /= flockmates.length;
        alignment_vector = normalize(alignment_vector);
        alignment_vector = multiplyVector(alignment_vector, alignment_factor);

        // C
        let cohesion_vector = new Vector(0, 0);

        flockmates.forEach((flockmate) => {
            cohesion_vector.x += flockmate.x;
            cohesion_vector.y += flockmate.y;
        });

        cohesion_vector.x /= flockmates.length;
        cohesion_vector.y /= flockmates.length;
        cohesion_vector.x -= this.x;
        cohesion_vector.y -= this.y;
        cohesion_vector = normalize(cohesion_vector);
        cohesion_vector = multiplyVector(cohesion_vector, cohesion_factor);

        // Simple summation of all weighted vectors (and the original heading to give a sense of momentum)
        this.heading = new Vector(separation_vector.x + alignment_vector.x + cohesion_vector.x + this.heading.x + target_vector.x, separation_vector.y + alignment_vector.y + cohesion_vector.y + this.heading.y + target_vector.y);
        this.heading = normalize(this.heading);

        this.updatePosition(this.x + this.heading.x, this.y + this.heading.y);
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
        this.shape = new createjs.Shape();
        this.shape.graphics.beginFill("yellow").drawPolyStar(0, 0, 8, 5, 0.6, -90);
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
        new_target = new Target(rand_x, rand_y);
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
    });

    stage.update();

    debug_shapes.forEach((shape) => {
        stage.removeChild(shape)
    });
    debug_shapes = [];
}

function pythagorean(x1, y1, x2, y2) {
    let x_distance = x2 - x1;
    let y_distance = y2 - y1;
    return Math.sqrt(Math.pow(x_distance, 2) + Math.pow(y_distance, 2));
}

function normalize(v) {
    let abs = magnitude(v);
    if (abs == 0){
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

function getRandomInt(min, max) {
    min = Math.ceil(min);
    max = Math.floor(max);
    return Math.floor(Math.random() * (max - min)) + min;
}

function magnitude(v){
    let abs = Math.pow(v.x, 2) + Math.pow(v.y, 2);
    if (abs == 0) {
        return 0;
    }
    return Math.sqrt(abs);
}
