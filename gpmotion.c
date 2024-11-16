
#include "m_pd.h"
#include "motion.h"

// TODO: add list outlet; messages for start/stop/reset calibration; process
// IMU data; get gravity, local, world, and player vectors

static t_class *gpmotion_class;

typedef struct _gpmotion {
    t_object  x_obj;
    GamepadMotion *gp;
} t_gpmotion;

void gpmotion_bang(t_gpmotion *x)
{
    post("gpmotion %p", x->gp);
}

void *gpmotion_new(void)
{
    t_gpmotion *x = (t_gpmotion *)pd_new(gpmotion_class);
    x->gp = gamepad_motion_init();

    return (void *)x;
}

void gpmotion_free(t_gpmotion *x)
{
    gamepad_motion_fini(x->gp);
}

void gpmotion_setup(void)
{
    gpmotion_class = class_new(gensym("gpmotion"),
                               (t_newmethod)gpmotion_new,
                               (t_method)gpmotion_free,
                               sizeof(t_gpmotion),
                               CLASS_DEFAULT, 0);

    class_addbang(gpmotion_class, gpmotion_bang);
}
