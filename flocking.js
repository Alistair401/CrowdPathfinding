let stage;
let canvas;
let population = 100;
let boid_radius = 100;

let boids = new Array();

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

        // A
        let local_avoidance_vector = new Vector(0, 0);

        flockmates.forEach((flockmate) => {
            local_avoidance_vector.x += flockmate.x - this.x;
            local_avoidance_vector.y += flockmate.y - this.y;
        });

        local_avoidance_vector.x *= -1;
        local_avoidance_vector.y *= -1;

        // B 
        let average_heading_vector = new Vector(0, 0);

        flockmates.forEach((flockmate) => {
            average_heading_vector.x += flockmate.heading.x;
            average_heading_vector.y += flockmate.heading.y;
        });

        average_heading_vector.x /= flockmates.length;
        average_heading_vector.y /= flockmates.length;

        // C
        let average_position_vector = new Vector(0, 0);

        flockmates.forEach((flockmate) => {
            average_position_vector.x += flockmate.x;
            average_position_vector.y += flockmate.y;
        });

        average_position_vector.x /= flockmates.length;
        average_position_vector.y /= flockmates.length;

        let direction_vector = new Vector(local_avoidance_vector.x + average_heading_vector.x + average_position_vector.x, local_avoidance_vector.y + average_heading_vector.y + average_position_vector.y);

        direction_vector = normalize(direction_vector);

        this.updatePosition(this.x + direction_vector.x, this.y + direction_vector.y);

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
}

function pythagorean(x1, y1, x2, y2) {
    let x_distance = x2 - x1;
    let y_distance = y2 - y1;
    return Math.sqrt(Math.pow(x_distance, 2) + Math.pow(y_distance, 2));
}

function normalize(v) {
    let abs = Math.pow(v.x, 2) + Math.pow(v.y, 2);
    abs = Math.sqrt(abs);
    return new Vector(v.x / abs, v.y / abs);
}
