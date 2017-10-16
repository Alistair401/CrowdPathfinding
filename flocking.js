let stage;
let canvas;
let population = 100;
let boid_radius = 75;
let cohesion_factor = 1.25;
let separation_factor = 1;
let alignment_factor = 0.5;
let debug = false;


let boids = new Array();

let debug_shapes = new Array();

class Boid {
    constructor(x, y) {
        this.x = x;
        this.y = y;

        this.heading = new Vector(0, 0);

        this.shape = new createjs.Shape()
        this.shape.graphics.beginFill("red").drawCircle(0, 0, 8);

        this.shape.x = this.x;
        this.shape.y = this.y;

        stage.addChild(this.shape);
    }

    update() {
        // Get local flockmates
        // A) Steer to avoid local flockmates
        // B) Steer towards the average heading of local flockmates
        // C) Steer to move towards the average position of flockmates

        let flockmates = new Array();

        boids.forEach((boid) => {
            if (boid == this) {
                return;
            }
            if (pythagorean(this.x, this.y, boid.x, boid.y) < boid_radius) {
                flockmates.push(boid);
            }
        });

        if (flockmates.length == 0) {
            return;
        }

        // A
        let separation_vector = new Vector(0, 0);

        flockmates.forEach((flockmate) => {
            separation_vector.x += flockmate.x - this.x;
            separation_vector.y += flockmate.y - this.y;
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
        cohesion_vector = multiplyVector(cohesion_vector, cohesion_factor)

        this.heading = new Vector(separation_vector.x + alignment_vector.x + cohesion_vector.x + this.heading.x, separation_vector.y + alignment_vector.y + cohesion_vector.y + this.heading.x);
        this.heading = normalize(this.heading);

        this.updatePosition(this.x + this.heading.x, this.y + this.heading.y);

        if (debug) {
            let a = new createjs.Shape();
            a.graphics.setStrokeStyle(1);
            a.graphics.beginStroke("white");
            a.graphics.moveTo(this.x, this.y);
            a.graphics.lineTo(this.x + separation_vector.x, this.y + separation_vector.y);
            a.graphics.endStroke();

            stage.addChild(a)
            debug_shapes.push(a)

            let b = new createjs.Shape();
            b.graphics.setStrokeStyle(1);
            b.graphics.beginStroke("blue");
            b.graphics.moveTo(this.x, this.y);
            b.graphics.lineTo(this.x + alignment_vector.x, this.y + alignment_vector.y);
            b.graphics.endStroke();

            stage.addChild(b)
            debug_shapes.push(b)

            let c = new createjs.Shape();
            c.graphics.setStrokeStyle(1);
            c.graphics.beginStroke("yellow");
            c.graphics.moveTo(this.x, this.y);
            c.graphics.lineTo(this.x + cohesion_vector.x, this.y + cohesion_vector.y);
            c.graphics.endStroke();

            stage.addChild(c)
            debug_shapes.push(c)
        }
    }

    updatePosition(x, y) {
        this.x = x;
        this.y = y;
        this.shape.x = x;
        this.shape.y = y;
    }
}

class Vector {
    constructor(x, y) {
        this.x = x;
        this.y = y;
    }
}

$(document).ready(() => {
    initCanvas();
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

    stage.update();

    debug_shapes.forEach((shape) => {
        stage.removeChild(shape)
    });
    debug_shapes = new Array();
}

function pythagorean(x1, y1, x2, y2) {
    let x_distance = x2 - x1;
    let y_distance = y2 - y1;
    return Math.sqrt(Math.pow(x_distance, 2) + Math.pow(y_distance, 2));
}

function normalize(v) {
    let abs = Math.pow(v.x, 2) + Math.pow(v.y, 2);
    if (abs == 0) {
        return v;
    }
    abs = Math.sqrt(abs);
    return new Vector(v.x / abs, v.y / abs);
}

function clamp(x, min, max) {
    return Math.max(Math.min(x, max), min);
}

function multiplyVector(v, x) {
    return new Vector(v.x * x, v.y * x)
}
