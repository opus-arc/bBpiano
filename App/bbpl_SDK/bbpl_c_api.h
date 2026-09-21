#ifndef bbpl_c_api_h
#define bbpl_c_api_h

#ifdef __cplusplus
extern "C" {
#endif

// ========================
// MIDI Service
// ========================

void midi_service_start(const char* midi_path);
void midi_service_stop(void);


// ========================
// Piano Service
// ========================

void piano_service_start(void);
void piano_service_stop(void);


// ========================
// Keyboard Service
// ========================

void keyboard_service_start(void);
void keyboard_service_stop(void);


// ========================
// Export Service
// ========================

void export_service_start(const char* midi_path);
void export_service_stop(void);


// ========================
// Record Service
// ========================

void record_service_start(void);
void record_service_stop(void);


// ========================
// Test Service
// ========================

void test_service_start(void);
void test_service_stop(void);


// ========================
// Internal Test Service
// ========================

void internal_test_service_start(void);
void internal_test_service_stop(void);

// ========================
// Shotdown All Service
// ========================
void close_all_service(void);

#ifdef __cplusplus
}
#endif

#endif /* bbpl_c_api_h */
