
#include <stdio.h>
#include <sys/types.h>
#include "py/runtime.h"
#include "py/obj.h"
#include "Adcs.h"
#include "extmod/ulab/code/ndarray.h"


static ndarray_obj_t* new_np_array( size_t len, size_t sz)
{

        ndarray_obj_t* ndarray = m_new_obj(ndarray_obj_t);
        
		ndarray->base.type = &ulab_ndarray_type;
		
		ndarray->dtype = NDARRAY_FLOAT;
        ndarray->boolean =  NDARRAY_NUMERIC;
        ndarray->ndim = 1;
        ndarray->len = len;
        ndarray->itemsize = sz;
        ndarray->shape[ULAB_MAX_DIMS - 1] = len;
        ndarray->strides[ULAB_MAX_DIMS - 1] = sz;

        //ndarray->array = src ;
		return ndarray;	
}

static void copy_to_c_array(ndarray_obj_t* ndarray, double* dst, size_t num)
{
	for (size_t i =0; i<num; i++) dst[i] = ((mp_float_t*)ndarray->array)[i];

}



/**
 * @brief: Get float array from python object
 * @param args arguments passed by interpreter
 * @param index index of args that stores the list
 * @param arry dest array
 */
static ndarray_obj_t* getNumpyArray(const mp_obj_t arg)
{

	if (mp_obj_is_type(arg,&ulab_ndarray_type)) {
		ndarray_obj_t* arry = MP_OBJ_TO_PTR(arg);
		return ndarray_copy_view(arry);
	}
	//else //mp_raise_TypeError(translate("input must be a dense ndarray hi"));
	uint8_t ndim = 1;
	size_t shape = 1;
	int32_t strides = 1;
	uint8_t dtype = NDARRAY_FLOAT;// 1;
	return ndarray_new_ndarray(ndim, &shape, &strides, dtype);
}

typedef struct _adcs_obj_t {
	mp_obj_base_t base;
	AdcsInfo adcs;

} adcs_obj_t;




STATIC mp_obj_t adcs_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
	

    adcs_obj_t *self = m_new_obj(adcs_obj_t);
	self->adcs.mode = 0;
	self->adcs.dt = 1;

	construct_ekf(&self->adcs.eInfo);
	construct_control(&self->adcs.cInfo);
	
	return MP_OBJ_FROM_PTR(self);
}



STATIC mp_obj_t adcs_single_iteration(size_t n_args, const mp_obj_t* args)
{
	//imu, gps, dipole
	double imu[6];
	double gps[3], dipole[3];

	ndarray_obj_t* imu_ndarray;
	ndarray_obj_t* gps_ndarray;

	adcs_obj_t* self = MP_OBJ_TO_PTR(args[0]);
	double dt = mp_obj_get_float(args[1]);
	imu_ndarray = getNumpyArray(args[2]);
	copy_to_c_array(imu_ndarray, imu, 6);
	self->adcs.dt = dt;
	if (n_args > 3) {
		gps_ndarray = getNumpyArray(args[3]);
		copy_to_c_array(gps_ndarray, gps, 3);
		single_iteration(&self->adcs, imu, gps, dipole);
	}
	else {
		
		single_iteration(&self->adcs, imu, NULL, dipole);
	}
	//ndarray_obj_t* dipole_ndarray = new_np_array(3, sizeof(float));
	//copy_to_np_array(dipole_ndarray, (uint8_t*) dipole, 3, sizeof(dipole));
	float ary[] = {dipole[0], dipole[1], dipole[2]};
	ndarray_obj_t* val = new_np_array(3, sizeof(float));
   	val->array = m_new0(uint8_t, 3*sizeof(float)); 
	memcpy(val->array, (uint8_t*)ary, 3*sizeof(float))	;

	printf("Single iteration dipole %f %f %f", dipole[0], dipole[1], dipole[2]);	
	return MP_OBJ_FROM_PTR(val);
}

STATIC mp_obj_t adcs_set_mode(mp_obj_t self_in, mp_obj_t newMode)
{
	adcs_obj_t* self = MP_OBJ_TO_PTR(self_in);

	self->adcs.mode = mp_obj_get_int(newMode);

	return newMode;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(adcs_single_iteration_obj,2, 4, adcs_single_iteration);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(adcs_set_mode_obj, adcs_set_mode);
/*
STATIC const mp_rom_map_elem_t adcs_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_single_iteration), MP_ROM_PTR(&adcs_single_iteration_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_mode), MP_ROM_PTR(&adcs_set_mode_obj) },
};
STATIC MP_DEFINE_CONST_DICT(adcs_locals_dict, adcs_locals_dict_table);
*/

STATIC void adcs_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_print_str(print, "adcs driver, why are you printing this?");
	
}

const mp_obj_type_t adcs_type = {
    { &mp_type_type },
    .name = MP_QSTR_c_adcs,
    .make_new = adcs_make_new,
    //.attr = adcs_attr,
    //.locals_dict = (mp_obj_dict_t*)&adcs_locals_dict,
	.print = adcs_print
};
/*
MP_DEFINE_CONST_OBJ_TYPE(
		adcs_type,
		MP_QSTR_c_adcs,
		MP_TYPE_FLAG_NONE,
		make_new, adcs_make_new,
		print, adcs_print
		);
*/

//register globals constants and dicts 
STATIC const mp_map_elem_t cAdcs_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_cAdcs) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_cAdcs_driver), (mp_obj_t)&adcs_type },	
    { MP_OBJ_NEW_QSTR(MP_QSTR_single_iteration), (mp_obj_t)(&adcs_single_iteration_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_set_mode), (mp_obj_t)(&adcs_set_mode_obj) },
};

STATIC MP_DEFINE_CONST_DICT (
    mp_module_cAdcs_globals,
    cAdcs_globals_table
);

const mp_obj_module_t cAdcs_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_cAdcs_globals,
};

MP_REGISTER_MODULE(MP_QSTR_cAdcs, cAdcs_user_cmodule, 1);
