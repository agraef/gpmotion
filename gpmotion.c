
#include "m_pd.h"
#include "motion.h"

static t_class *gpmotion_class;

static t_symbol *s_gravity, *s_local, *s_world, *s_player;

typedef struct _gpmotion {
    t_object  x_obj;
    GamepadMotion *gp;
    // yaw relax factor for player space
    float yaw_rf;
    // raw sensor data
    float ax, ay, az, gx, gy, gz;
    // processed gyro data (local, world, player space)
    float lx, ly, lz, wx, wy, px, py; // accumulated [deg]
    float slx, sly, slz, swx, swy, spx, spy; // sampled [deg/s]
    // Pd time of last sensor update
    double t;
    // state of sensor update, gyro mode
    int state, mode;
    t_outlet *out;
} t_gpmotion;

// gyro mode: 0 = accumulated rotation [deg], 1 = momentary angular velocity
// [deg/sec]

static void gpmotion_mode(t_gpmotion *x, t_floatarg f)
{
    x->mode = f != 0;
}

// yaw relax factor for player space (sensible range is about 1.15 .. 2 which
// corresponds to ~30° to ~60° of freedom for local space aiming); see
// http://gyrowiki.jibbsmart.com/blog:player-space-gyro-and-alternatives-explained#toc7

static void gpmotion_yawrf(t_gpmotion *x, t_floatarg f)
{
    // JoyShockMapper default
    if (f == 0.0f) f = 2.0f;
    // clamp to a sensible range
    if (f < 1.0f) f = 1.0f;
    if (f > 4.0f) f = 4.0f;
    x->yaw_rf = f;
}

// bang yields the normalized gravity vector (x, y, z)

static void gpmotion_bang(t_gpmotion *x)
{
    if (x->state >= 0 && x->t != 0.0f) {
        float xf, yf, zf;
        gamepad_motion_gravity(x->gp, &xf, &yf, &zf);
        t_atom ap[3];
        SETFLOAT(ap+0, xf);
        SETFLOAT(ap+1, yf);
        SETFLOAT(ap+2, zf);
        outlet_anything(x->out, s_gravity, 3, ap);
    }
}

// retrieve current angular velocity in deg/s (mode == 1), or rotation angle
// (mode == 0) in deg, (x, y, z) in local, or (x, y) in world/player space

// x is pitch, y is yaw, and z (in local space) is roll here. The world and
// player spaces each use pitch with a combination of yaw and roll relative to
// the gravity vector, but in different ways. These are preferred for
// gamepads, while local space usually works better with mobile devices. See
// http://gyrowiki.jibbsmart.com/blog for details.

static void gpmotion_local(t_gpmotion *x)
{
    if (x->state >= 0 && x->t != 0.0f) {
        float xf, yf, zf;
        if (x->mode)
            xf = x->slx, yf = x->sly, zf = x->slz;
        else
            xf = x->lx, yf = x->ly, zf = x->lz;
        t_atom ap[3];
        SETFLOAT(ap+0, xf);
        SETFLOAT(ap+1, yf);
        SETFLOAT(ap+2, zf);
        outlet_anything(x->out, s_local, 3, ap);
    }
}

static void gpmotion_world(t_gpmotion *x)
{
    if (x->state >= 0 && x->t != 0.0f) {
        float xf, yf;
        if (x->mode)
            xf = x->swx, yf = x->swy;
        else
            xf = x->wx, yf = x->wy;
        t_atom ap[2];
        SETFLOAT(ap+0, xf);
        SETFLOAT(ap+1, yf);
        outlet_anything(x->out, s_world, 2, ap);
    }
}

static void gpmotion_player(t_gpmotion *x)
{
    if (x->state >= 0 && x->t != 0.0f) {
        float xf, yf;
        if (x->mode)
            xf = x->spx, yf = x->spy;
        else
            xf = x->px, yf = x->py;
        t_atom ap[2];
        SETFLOAT(ap+0, xf);
        SETFLOAT(ap+1, yf);
        outlet_anything(x->out, s_player, 2, ap);
    }
}

// reset current rotation to origin (0, 0, 0)

static void gpmotion_reset(t_gpmotion *x)
{
    x->lx = 0.0f; x->ly = 0.0f; x->lz = 0.0f;
    x->wx = 0.0f; x->wy = 0.0f; x->px = 0.0f; x->py = 0.0f;
}

// calibration

static void gpmotion_start(t_gpmotion *x)
{
    gamepad_motion_start_calibration(x->gp);
}

static void gpmotion_stop(t_gpmotion *x)
{
    float xf, yf, zf;
    gamepad_motion_stop_calibration(x->gp);
    gamepad_motion_calibration(x->gp, &xf, &yf, &zf);
    post("calibration: %f %f %f", xf, yf, zf);
}

static void gpmotion_clear(t_gpmotion *x)
{
    float xf, yf, zf;
    gamepad_motion_reset_calibration(x->gp);
    gamepad_motion_calibration(x->gp, &xf, &yf, &zf);
    post("calibration: %f %f %f", xf, yf, zf);
}

// update the IMU data (every second time, assuming alternating accel and gyro
// values, as output by joyosc)

static void gpmotion_update(t_gpmotion *x)
{
    double t = x->t;
    double dt = clock_gettimesince(t)/1000.0;
    x->t = clock_getlogicaltime();
    if (t != 0.0f && x->t > t) {
        float xf, yf, zf;
        gamepad_motion_process(x->gp,
                               x->gx, x->gy, x->gz,
                               x->ax, x->ay, x->az, dt);
        // sample and accumulate gyro data
        gamepad_motion_local(x->gp, &xf, &yf, &zf);
        x->lx += xf*dt; x->ly += yf*dt; x->lz += zf*dt;
        x->slx = xf; x->sly = yf; x->slz = zf;
        gamepad_motion_world(x->gp, &xf, &yf);
        x->wx += xf*dt; x->wy += yf*dt;
        x->swx = xf; x->swy = yf;
        gamepad_motion_player(x->gp, &xf, &yf, x->yaw_rf);
        x->px += xf*dt; x->py += yf*dt;
        x->spx = xf; x->spy = yf;
    }
    x->state = 0;
}

static void gpmotion_accel(t_gpmotion *x, t_floatarg xf, t_floatarg yf, t_floatarg zf)
{
    x->ax = xf; x->ay = yf; x->az = zf;
    if (x->state <= 0)
        x->state++;
    else
        gpmotion_update(x);
}

static void gpmotion_gyro(t_gpmotion *x, t_floatarg xf, t_floatarg yf, t_floatarg zf)
{
    x->gx = xf; x->gy = yf; x->gz = zf;
    if (x->state <= 0)
        x->state++;
    else
        gpmotion_update(x);
}

static void *gpmotion_new(void)
{
    t_gpmotion *x = (t_gpmotion *)pd_new(gpmotion_class);
    x->gp = gamepad_motion_init();
    // yaw relax factor for player space (2.0 = 60° is the default being used
    // in JoyShockMapper, internal default is 1.41 = 45°)
    x->yaw_rf = 2.0f;
    x->ax = 0.0f; x->ay = 0.0f; x->az = 0.0f;
    x->gx = 0.0f; x->gy = 0.0f; x->gz = 0.0f;
    x->lx = 0.0f; x->ly = 0.0f; x->lz = 0.0f;
    x->wx = 0.0f; x->wy = 0.0f; x->px = 0.0f; x->py = 0.0f;
    x->slx = 0.0f; x->sly = 0.0f; x->slz = 0.0f;
    x->swx = 0.0f; x->swy = 0.0f; x->spx = 0.0f; x->spy = 0.0f;
    x->t = 0.0f;
    x->state = -1;
    x->mode = 0;
    x->out = outlet_new(&x->x_obj, 0);
    return (void *)x;
}

static void gpmotion_free(t_gpmotion *x)
{
    gamepad_motion_fini(x->gp);
}

void gpmotion_setup(void)
{
    s_gravity = gensym("gravity");
    s_local = gensym("local");
    s_world = gensym("world");
    s_player = gensym("player");

    gpmotion_class = class_new(gensym("gpmotion"),
                               (t_newmethod)gpmotion_new,
                               (t_method)gpmotion_free,
                               sizeof(t_gpmotion),
                               CLASS_DEFAULT, 0);

    class_addmethod(gpmotion_class,
                    (t_method)gpmotion_mode, gensym("mode"), A_DEFFLOAT, 0);
    class_addmethod(gpmotion_class,
                    (t_method)gpmotion_yawrf, gensym("yawrf"), A_DEFFLOAT, 0);
    class_addbang(gpmotion_class, gpmotion_bang);
    class_addmethod(gpmotion_class,
                    (t_method)gpmotion_local, s_local, 0);
    class_addmethod(gpmotion_class,
                    (t_method)gpmotion_world, s_world, 0);
    class_addmethod(gpmotion_class,
                    (t_method)gpmotion_player, s_player, 0);
    class_addmethod(gpmotion_class,
                    (t_method)gpmotion_reset, gensym("reset"), 0);
    class_addmethod(gpmotion_class,
                    (t_method)gpmotion_start, gensym("start"), 0);
    class_addmethod(gpmotion_class,
                    (t_method)gpmotion_stop, gensym("stop"), 0);
    class_addmethod(gpmotion_class,
                    (t_method)gpmotion_clear, gensym("clear"), 0);
    class_addmethod(gpmotion_class, (t_method)gpmotion_accel, gensym("accel"), A_FLOAT, A_FLOAT, A_FLOAT, 0);
    class_addmethod(gpmotion_class, (t_method)gpmotion_gyro, gensym("gyro"), A_FLOAT, A_FLOAT, A_FLOAT, 0);
}
