#version 450

const double a = -73.64196;
const float b = 975;
const int c = 25;
int iter_map(double w) {
    return int(b * exp(float(w * a))) + c;
}

layout (set = 3, binding = 0) uniform Viewport {
    dvec2 offset;
    double width;
    double ratio;
} view;

layout(set = 3, binding = 1) uniform UResolution {
    vec2 u_resolution;
} uResolution;

layout (set = 2, binding = 0) readonly buffer ColorMap {
    vec3 colors[256];
} colors;

layout (location = 0) out vec4 FragColor;

int ITER_COUNT = iter_map(view.width);
const double BREAKOUT = 4.;

vec3 fromColormap(int i) {
    int idx = i / ITER_COUNT * 256;
    return colors.colors[idx];
}

int iter_check(dvec2 point) {
    dvec2 z = point;
    int i;
    for(i = 0; i < ITER_COUNT; ++i) {
        if (dot(z, z) >= BREAKOUT * BREAKOUT) break;
        z = dvec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + point;
    }
    return i;
}

void main() {
    vec3 color;
    dvec2 pos = gl_FragCoord.xy;
    dvec2 point = dvec2 (
        pos.x / uResolution.u_resolution.x * view.width + view.offset.x,
        pos.y / uResolution.u_resolution.y * view.width * view.ratio - view.offset.y
    );
    int i;
    
    double q = (point.x - 0.25)*(point.x-0.25) + point.y * point.y;
    
    if (point.x * point.x + point.y * point.y > 4) {
        i = 0;
    }
    // Cardioid check
    else if (q*(q+(point.x-0.25)) < 0.25 * point.y * point.y) {
        i = ITER_COUNT;
    }
    // Bulb check
    else if ((point.x + 1)*(point.x + 1) + point.y * point.y < 0.0625) {
        i = ITER_COUNT;
    }
    // Iteration check
    else {
        i = iter_check(point);
    }

    
    float t = float(i) / float(ITER_COUNT);
    color = colors.colors[int(t * 255)];
    FragColor = vec4(color.xyz, 1);//vec4(t, t*t, t*t*t, 1.0);
}