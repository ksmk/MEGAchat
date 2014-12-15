#ifndef GUICALLMARSHALLER_H
#define GUICALLMARSHALLER_H

/* This is a plain C header */
#ifdef __cplusplus
  #define MEGA_GCM_EXTERNC extern "C"
#else
  #define MEGA_GCM_EXTERNC
#endif

#ifdef _WIN32
#define MEGA_GCM_EXPORT MEGA_GCM_EXTERNC __declspec(dllexport)
#else
#define MEGA_GCM_EXPORT MEGA_GCM_EXTERNC __attribute__ ((visibility ("default")))
#endif

struct megaMessage;
typedef void(*megaMessageFunc)(struct megaMessage*);

#ifdef __cplusplus
extern "C"
{
#endif

/** The mechanism supports marshalling of a plain C function with signature void(void*)
 * The Message structure carries the function pointer and the data is a pointer to
 * the Message struct itself. The actual object passed is derived from \c Message and can
 * contain arbitrary data, so \c struct \c Message acts as the header of the actual structure
 * passed. As the handler knows the actual type of the structure, it casts it to the proper
 * derived type and accesses the additional data after the \c Message header. In other words,
 * this is plain C polymorphism, and this API is plain C to make possible interoperation
 * between modules built with different compilers, even different languages.
 *
 * All memory associated with the message object is managed on the same side of the
 * shared-object boundary (the one that posts the message),
 * because only that side knows the exact type of the message
 * object, and also because each shared object is likely to
 * have its own memory management and even be built with a different compiler (C++ ABIs
 * are incompatible).
*/

struct megaMessage
{
    megaMessageFunc func;
#ifdef __cplusplus
//if we don't provice an initializing constructor, operator new() will initialize func
//to NULL, and then we will overwrite it, which is inefficient.
    megaMessage(megaMessageFunc aFunc): func(aFunc){}
#endif
};
//enum {kMegaMsgMagic = 0x3e9a3591};

/** This is the type of the function that posts a megaMessage to the GUI thread */
typedef void (*GcmPostFunc)(void*);

/** This function posts an opaque \c void* to the application's (GUI) message loop
* and that message is then received by the application's main (GUI) thread and sent to
* the \c megaProcessMessage(void*) to be handled by the receiving code.
* \warning This function must be provided by the user and is specific to the GUI framework
* used in the app. It is called by various threads when they need to execute a function
* call by the main (GUI) thread.
*/

void megaPostMessageToGui(void* msg);

/** When the application's main (GUI) thread receives a message posted by
 * megaPostMessageToGui(), it must forward the \c void* pointer
 * from that message to this function for further processing. This function is
 * called by a handler in the app's (GUI) event/message loop (or equivalent, such as
 * a QT slot called on the GUI thread) and normally it should be called only
 * from one place in the code that serves as a bridge between the native message
 * loop and the mega messaging system.
* \warning Must be called only from the GUI thread
*/

static inline void megaProcessMessage(void* vptr)
{
    struct megaMessage* msg = (struct megaMessage*)vptr;
    msg->func(msg);
}

#ifdef __cplusplus
} //end extern "C"
#endif

#endif // GUICALLMARSHALLER_H