
#include "m_pd.h"

static t_class *gpmotion_class;

typedef struct _gpmotion {
    t_object  x_obj;
} t_gpmotion;

void gpmotion_bang(t_gpmotion *x)
{
    post("Hello world !!");
}

void *gpmotion_new(void)
{
    t_gpmotion *x = (t_gpmotion *)pd_new(gpmotion_class);

    return (void *)x;
}

void gpmotion_setup(void)
{
    gpmotion_class = class_new(gensym("gpmotion"),
                               (t_newmethod)gpmotion_new,
                               0, sizeof(t_gpmotion),
                               CLASS_DEFAULT, 0);

    class_addbang(gpmotion_class, gpmotion_bang);
}
