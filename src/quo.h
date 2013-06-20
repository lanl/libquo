/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 */

#ifndef QUO_H_INCLUDED
#define QUO_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define LIBQUO 1

struct QUO_t;
typedef struct QUO_t QUO_t;

/** quo return codes */
enum {
    QUO_SUCCESS = 0,
    QUO_SUCCESS_ALREADY_DONE,
    QUO_ERR,
    QUO_ERR_SYS,
    QUO_ERR_OOR,
    QUO_ERR_INVLD_ARG,
    QUO_ERR_CALL_BEFORE_INIT,
    QUO_ERR_TOPO,
    QUO_ERR_MPI,
    QUO_ERR_NOT_SUPPORTED,
    QUO_ERR_POP,
    QUO_ERR_NOT_FOUND
};

typedef enum {
    QUO_OBJ_MACHINE = 0,
    QUO_OBJ_NODE,
    QUO_OBJ_SOCKET,
    QUO_OBJ_CORE,
    QUO_OBJ_PU
} QUO_obj_type_t;

typedef enum {
    QUO_BIND_PUSH_PROVIDED = 0,
    QUO_BIND_PUSH_OBJ
} QUO_bind_push_policy_t;

/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */
/* quo api */
/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */

/**
 * this routine can be called before QUO_init.
 */
int
QUO_version(int *version,
            int *subversion);

/* 1st call */
int
QUO_construct(QUO_t **q);

/* 2nd call */
int
QUO_init(QUO_t *q);

/* 2nd-to-last call */
int
QUO_finalize(QUO_t *q);

/* last call - call before MPI_Finalize */
int
QUO_destruct(QUO_t *q);

int
QUO_initialized(const QUO_t *q,
                int *out_initialized);

int
QUO_ranks_on_node(const QUO_t *q,
                  int *out_nranks,
                  int **out_ranks);

int
QUO_node_topo_stringify(const QUO_t *q,
                        char **out_str);

int
QUO_get_nobjs_by_type(const QUO_t *q,
                      QUO_obj_type_t target_type,
                      int *out_nobjs);

int
QUO_get_nobjs_in_type_by_type(const QUO_t *q,
                              QUO_obj_type_t in_type,
                              int in_type_index,
                              QUO_obj_type_t type,
                              int *out_result);
/**
 * returns whether or not my current binding policy covers a particular system
 * hardware resource.
 */
int
QUO_cur_cpuset_in_type(const QUO_t *q,
                       QUO_obj_type_t type,
                       int in_type_index,
                       int *out_result);

int
QUO_smpranks_in_type(const QUO_t *q,
                     QUO_obj_type_t type,
                     int in_type_index,
                     int *n_out_smpranks,
                     int **out_smpranks);

int
QUO_nsockets(const QUO_t *q,
             int *out_nsockets);

int
QUO_ncores(const QUO_t *q,
           int *out_ncores);

int
QUO_npus(const QUO_t *q,
         int *out_npus);

int
QUO_bound(const QUO_t *q,
          int *bound);

int
QUO_stringify_cbind(const QUO_t *q,
                    char **cbind_str);

int
QUO_nnodes(const QUO_t *q,
           int *out_nodes);

int
QUO_nnoderanks(const QUO_t *q,
               int *out_nnoderanks);

int
QUO_noderank(const QUO_t *q,
             int *out_noderank);

int
QUO_bind_push(QUO_t *q,
              QUO_bind_push_policy_t policy,
              QUO_obj_type_t type,
              int obj_index);
int
QUO_bind_pop(QUO_t *q);

void
QUO_free(void *target);

#ifdef __cplusplus
}
#endif

/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */
/* fortran stuff */
/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */
#define QUO_fint int

#endif
