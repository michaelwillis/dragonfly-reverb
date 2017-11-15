typedef struct _SR2_(SRC_STATE_tag) _SR2_(SRC_STATE);

typedef struct
{
  _sr2_float_t *data_in, *data_out;
  long input_frames, output_frames;
  long input_frames_used, output_frames_gen;
  long end_of_input;
  double src_ratio;
} _SR2_(SRC_DATA);

typedef struct
{
  long	frames;
  _sr2_float_t	*data_in;
} _SR2_(SRC_CB_DATA);

typedef long (* _SR2_(src_callback_t) ) (void *cb_data, _sr2_float_t **data);
_SR2_(SRC_STATE) * _SR2_(src_new) (long converter_type, long channels, long *error);
_SR2_(SRC_STATE) * _SR2_(src_callback_new) (_SR2_(src_callback_t) func,
					  long converter_type, long channels,
					  long *error, void* cb_data);
_SR2_(SRC_STATE) * _SR2_(src_delete) (_SR2_(SRC_STATE) *state);
long _SR2_(src_process) (_SR2_(SRC_STATE) *state, _SR2_(SRC_DATA) *data);
long _SR2_(src_callback_read) (_SR2_(SRC_STATE) *state, double src_ratio,
			       long frames, _sr2_float_t *data);
long _SR2_(src_simple) (_SR2_(SRC_DATA) *data, long converter_type,
			long channels);
long _SR2_(src_set_ratio) (_SR2_(SRC_STATE) *state, double new_ratio) ;
long _SR2_(src_reset) (_SR2_(SRC_STATE) *state) ;
long _SR2_(src_error) (_SR2_(SRC_STATE) *state) ;
_sr2_float_t _SR2_(fmod_one) (_sr2_float_t x);
