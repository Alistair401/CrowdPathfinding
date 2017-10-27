let population = 50;
let groups = 3;
let iterations = 100;
let debug = true;

let stage;
let canvas;

let boids = new Array();

let debug_shapes = new Array();

class Boid {
    constructor(x, y) {
        this.x = x;
        this.y = y;

        this.shape = new createjs.Shape();
        this.shape.graphics.beginFill("red").drawCircle(0, 0, 8);

        this.shape.x = this.x;
        this.shape.y = this.y;

        stage.addChild(this.shape);
    }

    update() {

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
    initBoids();

    kmeans(iterations, groups);

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

    stage.update();

    //    debug_shapes.forEach((shape) => {
    //        stage.removeChild(shape)
    //    });
    //    debug_shapes = new Array();
}

function kmeans(i, k) {
    if (i == 0 || k == 0) {
        return;
    }

    let centers = new Array();
    let random_boid = boids[getRandomInt(0, boids.length)];
    let new_center = new Vector(random_boid.x, random_boid.y);
    centers.push(new_center);

    // kmeans++ to choose centers
    for (let center_index = 1; center_index < k; center_index++) {
        let weight_array = new Array(boids.length);
        weight_array.fill(Number.POSITIVE_INFINITY);

        for (let boid_index = 0; boid_index < boids.length; boid_index++) {
            let current_boid = boids[boid_index];
            centers.forEach((center) => {
                let distance_squared = get_weight(current_boid.x, current_boid.y, center.x, center.y);
                if (distance_squared < weight_array[boid_index]) {
                    weight_array[boid_index] = distance_squared;
                }
            });
        }

        let cumulative_weight_array = new Array(boids.length)
        cumulative_weight_array[0] = weight_array[0];
        for (let boid_index = 1; boid_index < boids.length; boid_index++) {
            cumulative_weight_array[boid_index] = cumulative_weight_array[boid_index - 1] + weight_array[boid_index];
        }

        random_weight = Math.random() * cumulative_weight_array[cumulative_weight_array.length - 1];

        for (let boid_index = 0; boid_index < boids.length; boid_index++) {
            if (random_weight < cumulative_weight_array[boid_index]) {
                let selected_boid = boids[boid_index];
                centers.push(new Vector(selected_boid.x, selected_boid.y));
                break;
            }
        }
    }

    // kmeans iterations
    for (let iter = 1; iter < i; iter++) {
        let closest_center_array = new Array(boids.length);
        for (let boid_index = 0; boid_index < boids.length; boid_index++) {
            let current_boid = boids[boid_index]
            let closest_center = 0;
            let closest_distance = pythagorean(current_boid.x, current_boid.y, centers[0].x, centers[0].y);
            for (let center_index = 1; center_index < centers.length; center_index++) {
                let current_center = centers[center_index];
                if (pythagorean(current_boid.x, current_boid.y, current_center.x, current_center.y) < closest_distance) {
                    closest_center = center_index;
                }
            }
            closest_center_array[boid_index] = closest_center;
        }
        let mean_x_array = new Array(centers.length);
        mean_x_array.fill(0);
        let mean_y_array = new Array(centers.length);
        mean_y_array.fill(0);
        let count_array = new Array(centers.length);
        count_array.fill(0);
        for (let boid_index = 0; boid_index < boids.length; boid_index++) {
            mean_x_array[closest_center_array[boid_index]] += boids[boid_index].x
            mean_y_array[closest_center_array[boid_index]] += boids[boid_index].y
            count_array[closest_center_array[boid_index]] += 1;

        }
        for (let center_index = 0; center_index < centers.length; center_index++) {
            if (!count_array[center_index]) {
                continue;
            }
            mean_x_array[center_index] /= count_array[center_index];
            mean_y_array[center_index] /= count_array[center_index];

            centers[center_index].x = mean_x_array[center_index]
            centers[center_index].y = mean_y_array[center_index]
        }
    }

    centers.forEach((center) => {
        let debug_shape = new createjs.Shape();
        debug_shape.graphics.beginFill("white").drawCircle(0, 0, 8);

        debug_shape.x = center.x;
        debug_shape.y = center.y;

        stage.addChild(debug_shape);
        debug_shapes.push(debug_shape);
    });


    function get_weight(x1, y1, x2, y2) {
        let x_distance = x2 - x1;
        let y_distance = y2 - y1;
        return Math.pow(x_distance, 2) + Math.pow(y_distance, 2);
    }

}

function pythagorean(x1, y1, x2, y2) {
    if (x1 == y1 && x2 == y2) {
        return 0;
    }
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
