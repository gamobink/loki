#ifndef loki_services_h
#define loki_services_h


#include "loki.h"


/* services */

LKMOD_API lk_Handler loki_service_loader;
LKMOD_API lk_Handler loki_service_log;
LKMOD_API lk_Handler loki_service_monitor;
LKMOD_API lk_Handler loki_service_socket;
LKMOD_API lk_Handler loki_service_timer;


/* loader interface */

#ifndef LK_PATH
# ifdef _WIN32
#   define LK_PATH "!\\services\\?.dll;" "!\\..\\services\\?.dll;" "!\\?.dll;" \
                   ".\\services\\?.dll;" "..\\services\\?.dll;" ".\\?.dll"
# else
#   define LK_PATH "services/?.so;" "../services/?.so;" "./?.so"
# endif
#endif /* LK_PATH */

typedef struct lk_Loader lk_Loader;

typedef int lk_LoaderHandler (lk_State *S, void *ud, lk_Loader *l, const char *name);

LK_API int  lk_loaderror  (lk_Loader *loader, const char *msg, ...);
LK_API int  lk_loadverror (lk_Loader *loader, const char *msg, va_list l);
LK_API void lk_sethandler (lk_Loader *loader, lk_Handler *h,
                           void *data, lk_Module mod);

LK_API void lk_preload (lk_Service *svr, const char *name, lk_Handler *h);

LK_API lk_Service *lk_require (lk_Service *svr, const char *name);

LK_API void lk_addpath   (lk_Service *svr, const char *path);
LK_API void lk_addloader (lk_Service *svr, lk_LoaderHandler *h, void *ud);


/* monitor interface */

typedef struct lk_Monitor lk_Monitor;

struct lk_Monitor {
    lk_State *S;
    void *ud;
    void (*on_require) (lk_Monitor *cbs, lk_Service *src, lk_Service *dst);
    void (*on_weak)    (lk_Monitor *cbs, lk_Service *svr);
    void (*on_open)    (lk_Monitor *cbs, lk_Service *src, lk_Service *dst);
    void (*on_close)   (lk_Monitor *cbs, lk_Service *svr);
};

LK_API void lk_addmonitor (lk_Service *svr, lk_Monitor *cbs);
LK_API void lk_delmonitor (lk_Service *svr, lk_Monitor *cbs);


/* timer interface */

#ifndef lk_Time
# ifdef LOKI_USE_64BIT_TIMER
typedef unsigned long long lk_Time;
# else
typedef unsigned lk_Time;
# endif
# define lk_Time lk_Time
#endif /* lk_Time */

typedef struct lk_Timer lk_Timer;

typedef lk_Time lk_TimerHandler (lk_State *S, void *ud, lk_Timer *timer, lk_Time delayed);

LK_API lk_Time lk_time(void);

LK_API lk_Timer *lk_newtimer (lk_Service *svr, lk_TimerHandler *h, void *ud);
LK_API void      lk_deltimer (lk_Timer *timer);

LK_API void lk_starttimer  (lk_Timer *timer, lk_Time delayms);
LK_API void lk_canceltimer (lk_Timer *timer);


/* socket interface */

typedef struct lk_Accept lk_Accept;
typedef struct lk_Tcp    lk_Tcp;
typedef struct lk_Udp    lk_Udp;

typedef void lk_AcceptHandler (lk_State *S, void *ud, unsigned err, lk_Accept *accept, lk_Tcp *tcp);
typedef void lk_ConnectHandler (lk_State *S, void *ud, unsigned err, lk_Tcp *tcp);
typedef void lk_UdpBindHandler (lk_State *S, void *ud, unsigned err, lk_Udp *udp);

typedef size_t lk_HeaderHandler   (lk_State *S, void *ud, lk_Tcp *tcp, const char *buff, size_t len);
typedef void   lk_PacketHandler   (lk_State *S, void *ud, lk_Tcp *tcp, const char *buff, size_t len);
typedef void   lk_RecvFromHandler (lk_State *S, void *ud, lk_Udp *udp, unsigned err,
                                   const char *buff, unsigned count,
                                   const char *addr, unsigned port);

LK_API void lk_setonheader (lk_Service *svr, lk_HeaderHandler *h, void *ud);
LK_API void lk_setonpacket (lk_Service *svr, lk_PacketHandler *h, void *ud);
LK_API void lk_setonudpmsg (lk_Service *svr, lk_RecvFromHandler *h, void *ud);

LK_API lk_Accept *lk_newaccept (lk_Service *svr, lk_AcceptHandler *h, void *ud);
LK_API void       lk_delaccept (lk_Accept *accept);

LK_API void lk_listen (lk_Accept *accept, const char *addr, unsigned port);

LK_API void lk_connect (lk_Service *svr, const char *addr, unsigned port,
                        lk_ConnectHandler *h, void *ud);
LK_API void lk_deltcp  (lk_Tcp *tcp);

LK_API void *lk_gettcpdata (lk_Tcp *tcp);
LK_API void  lk_settcpdata (lk_Tcp *tcp, void *data);

LK_API void lk_send (lk_Tcp *tcp, const char *buff, unsigned size);

LK_API void lk_bindudp (lk_Service *svr, const char *addr, unsigned port,
                        lk_UdpBindHandler *h, void *ud);
LK_API void lk_deludp  (lk_Udp *udp);

LK_API void lk_sendto (lk_Udp *udp, const char *buff, unsigned len,
                       const char *addr, unsigned port);


#endif /* loki_services_h */

