let population = 100;
let boid_radius = 75;
let target_count = 10;

let cohesion_factor = 0.2;
let separation_factor = 0.2;
let alignment_factor = 0.2;
let target_factor = 0.4;
let heading_factor = 0.8;

let graph = [];
let graph_width = 50;
let graph_height = 50;

let stage;
let canvas;

let targets = [];

let boids = [];

class Boid {
    constructor(x, y) {
        this.x = x;
        this.y = y;

        let target_index = getRandomInt(0, target_count);
        this.target = targets[target_index];

        this.heading = new Vector(0, 0);

        this.path = [];

        this.flockmates = [];

        this.shape = new createjs.Shape();
        this.shape.graphics.beginFill("red").drawCircle(0, 0, 8);

        this.shape.x = this.x;
        this.shape.y = this.y;

        stage.addChild(this.shape);
    }

    update() {

        this.heading = multiplyVector(this.heading, heading_factor);
        let target_vector = new Vector(0, 0);
        let separation_vector = new Vector(0, 0);
        let alignment_vector = new Vector(0, 0);
        let cohesion_vector = new Vector(0, 0);

        this.updateFlockmates();

        if (this.flockmates.length > 0) {
            this.flockmates.forEach((flockmate) => {
                if (pythagorean(this.x, this.y, flockmate.x, flockmate.y) < boid_radius / 2) {
                    separation_vector.x += flockmate.x - this.x;
                    separation_vector.y += flockmate.y - this.y;
                }
                alignment_vector.x += flockmate.heading.x;
                alignment_vector.y += flockmate.heading.y;
                cohesion_vector.x += flockmate.x;
                cohesion_vector.y += flockmate.y;
            });

            separation_vector.x *= -1;
            separation_vector.y *= -1;
            separation_vector = normalize(separation_vector);
            separation_vector = multiplyVector(separation_vector, separation_factor);

            alignment_vector.x /= this.flockmates.length;
            alignment_vector.y /= this.flockmates.length;
            alignment_vector = normalize(alignment_vector);
            alignment_vector = multiplyVector(alignment_vector, alignment_factor);

            cohesion_vector.x /= this.flockmates.length;
            cohesion_vector.y /= this.flockmates.length;
            cohesion_vector.x -= this.x;
            cohesion_vector.y -= this.y;
            cohesion_vector = normalize(cohesion_vector);
            cohesion_vector = multiplyVector(cohesion_vector, cohesion_factor);
        } else {
            if (this.path.length === 0) {
                let current_node = pointToGraph(this.x, this.y);
                let target_node = pointToGraph(this.target.x, this.target.y);
                this.path = aStar(current_node, target_node, graph);
            }
            target_vector = this.getPathVector();
        }

        // Simple summation of all weighted vectors (and the original heading to give a sense of momentum)
        this.heading = new Vector(
            separation_vector.x + alignment_vector.x + cohesion_vector.x + target_vector.x + this.heading.x,
            separation_vector.y + alignment_vector.y + cohesion_vector.y + target_vector.y + this.heading.y
        );
        this.heading.x = clamp(this.heading.x, -1, 1);
        this.heading.y = clamp(this.heading.y, -1, 1);

        this.updatePosition(this.x + this.heading.x, this.y + this.heading.y);
    }

    updateHeading() {

    }

    updatePosition(x, y) {
        this.x = x;
        this.y = y;
        this.shape.x = x;
        this.shape.y = y;
    }

    updateFlockmates() {
        for (let boid of boids) {
            if (boid === this) {
                continue;
            }
            if (pythagorean(this.x, this.y, boid.x, boid.y) < boid_radius) {
                this.flockmates.push(boid);
            }
        }
    }

    getPathVector() {
        let target_vector = new Vector(this.path[0].x - this.x, this.path[0].y - this.y);
        target_vector = normalize(target_vector);
        target_vector = multiplyVector(target_vector, target_factor);
        return target_vector;
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

$(document).ready(() => {
    initCanvas();
    initGraph();
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
        let new_target = new Target(rand_x, rand_y);
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

function initGraph() {
    let x_scale = canvas.width / graph_width;
    let y_scale = canvas.height / graph_height;
    for (let x = 0; x < graph_width; x++) {
        graph.push([]);
        for (let y = 0; y < graph_height; y++) {
            graph[x].push(new GraphNode((x_scale / 2) + (x * x_scale), (y_scale / 2) + (y * y_scale), x, y));
        }
    }
}

function handleTick() {
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;

    boids.forEach((boid) => {
        boid.update();
    });

    stage.update();
}

function pointToGraph(x, y) {
    let x_scale = canvas.width / graph_width;
    let y_scale = canvas.height / graph_height;

    let x_val = Math.round(((x - (x_scale / 2)) / canvas.width) * graph_width);
    let y_val = Math.round(((y - (y_scale / 2)) / canvas.height) * graph_height);

    x_val = clamp(x_val, 0, graph_width - 1);
    y_val = clamp(y_val, 0, graph_height - 1);
    return graph[x_val][y_val];
}


