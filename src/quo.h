/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 */

#ifndef QUO_H_INCLUDED
#define QUO_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* in case you need this */
#define LIBQUO 1

/** opaque quo context */
struct QUO_t;
typedef struct QUO_t QUO_t;

/** quo return codes */
enum {
    /* success */
    QUO_SUCCESS = 0,
    /* success, but already done */
    QUO_SUCCESS_ALREADY_DONE,
    /* general error */
    QUO_ERR,
    /* system error */
    QUO_ERR_SYS,
    /* out of resources error */
    QUO_ERR_OOR,
    /* invalid argument provided to library */
    QUO_ERR_INVLD_ARG,
    /* library call before QUO_init was called */
    QUO_ERR_CALL_BEFORE_INIT,
    /* topology error */
    QUO_ERR_TOPO,
    /* mpi error */
    QUO_ERR_MPI,
    /* action not supported */
    QUO_ERR_NOT_SUPPORTED,
    /* *pop error */
    QUO_ERR_POP,
    /* the thing that you were looking for wasn't found */
    QUO_ERR_NOT_FOUND
};

/** hardware resource types */
typedef enum {
    /* the machine */
    QUO_OBJ_MACHINE = 0,
    /* NUMA node */
    QUO_OBJ_NODE,
    /* socket */
    QUO_OBJ_SOCKET,
    /* core */
    QUO_OBJ_CORE,
    /* processing unit (e.g. hardware thread) */
    QUO_OBJ_PU
} QUO_obj_type_t;

/** push policies that influence QUO_bind_push behavior. */
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
 * libquo version query routine.
 *
 * @param version - major version (OUT)
 *
 * @param subversion - subversion (OUT)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * NOTES: this routine can be called before QUO_init.
 */
int
QUO_version(int *version,
            int *subversion);

/**
 * libquo context handle construction routine.
 *
 * @param q - reference to a new, unconstructed QUO_t pointer. (OUT)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * NOTES: this is typically the first "real" call into the library. a
 * relatively inexpensive routine that can be called before MPI_Init. call
 * QUO_destruct to free returned resources.
 *
 * EXAMPLE (c):
 * QUO_t *quo = NULL;
 * if (QUO_SUCCESS != QUO_construct(&quo)) {
 *     // error handling //
 * }
 */
int
QUO_construct(QUO_t **q);

/**
 * libquo context handle initialization routine.
 *
 * @param q - constructed QUO_t context pointer. (IN)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * NOTES: this is typically the second "real" call into the library. a
 * relatively EXPENSIVE routine that must be called after MPI_Init. call
 * QUO_finalize to "finalize" all services that were started with this call. ALL
 * participating processes (i.e. everyone in MPI_COMM_WORLD) must call this
 * routine at the same time, as this is a collective operation across a
 * libquo-maintained duplicate of MPI_COMM_WORLD.
 *
 * EXAMPLE (c):
 * // ... //
 * if (QUO_SUCCESS != QUO_init(quo)) {
 *     // error handling //
 * }
 */
int
QUO_init(QUO_t *q);

/**
 * libquo context handle finalization routine.
 *
 * @param q - constructed QUO_t context pointer. (IN)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * NOTES: this is typically the second to the last "real" call into the library.
 * a relatively inexpensive routine that must be called BEFORE MPI_Finalize.
 * call QUO_finalize to "finalize" all services that were started with QUO_init.
 * once a call to this routine is made, it is an error to use any libquo
 * services associated with the finalized libquo context from any other
 * participating process.
 *
 * EXAMPLE (c):
 * // ... //
 * if (QUO_SUCCESS != QUO_finalize(quo)) {
 *     // error handling //
 * }
 */
int
QUO_finalize(QUO_t *q);

/**
 * libquo context handle destruction routine.
 *
 * @param q - constructed and initialized QUO_t context pointer. (IN)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * NOTES: this is typically the last "real" call into the library.  a relatively
 * inexpensive routine that must be called BEFORE MPI_Finalize.  call
 * QUO_destruct to release all resources that were allocated with QUO_construct.
 * once a call to this routine is made, it is an error to use any libquo
 * services associated with the destructed libquo context from any other
 * participating process.
 *
 * EXAMPLE (c):
 * // ... //
 * if (QUO_SUCCESS != QUO_destruct(quo)) {
 *     // error handling //
 * }
 */
int
QUO_destruct(QUO_t *q);

/**
 * libquo context handle query routine.
 *
 * @param q - QUO_t context pointer. (IN)
 *
 * @param out_initialized - flag indicating whether or not the provided context
 *                          has been initialized.
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * NOTES: can only be safely called if the provided context has been
 * constructed.
 */
int
QUO_initialized(const QUO_t *q,
                int *out_initialized);

/**
 * libquo context handle query routine that returns the number of ranks that
 * are on the caller's node and their corresponding MPI_COMM_WORLD ranks. the
 * number and returned array also include the calling rank's information.
 *
 * @param q - constructed and initialized QUO_t context pointer. (IN)
 *
 * @param out_nranks - the number of ranks that are on the node (OUT)
 *
 * @param out_ranks - an array of MPI_COMM_WORLD ranks that are on the node.
 *                    *out_ranks must be freed by a call to free(3). (OUT)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * EXAMPLE (c):
 * // ... //
 * int nnoderanks = 0;
 * int *noderanks = NULL;
 * if (QUO_SUCCESS != QUO_ranks_on_node(quo, &nnoderanks, &noderanks))
 *     // error handling //
 * }
 * // ... //
 * free(noderanks);
 */
int
QUO_ranks_on_node(const QUO_t *q,
                  int *out_nranks,
                  int **out_ranks);

/**
 * libquo context handle query routine that returns a string that represents the
 * system's topology.
 *
 * @param q - constructed and initialized QUO_t context pointer. (IN)
 *
 * @param out_str - the system's topology string. *out_str must be freed by a
 *                  call to free(3). (OUT)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * EXAMPLE (c):
 * // ... //
 * char *topostr = NULL;
 * if (QUO_SUCCESS != QUO_node_topo_stringify(q, &topostr)) {
 *     // error handling //
 * }
 * printf("%s\n", topostr);
 * free(topostr);
 */
int
QUO_node_topo_stringify(const QUO_t *q,
                        char **out_str);

/**
 * libquo context handle query routine that returns the total number of hardware
 * resource objects that are on the caller's system.
 *
 * @param q - constructed and initialized QUO_t context pointer. (IN)
 *
 * @param target_type - hardware object type that is being queried. (IN)
 *
 * @param out_nobjs - total number of hardware object types found on the
 *                    system. (OUT)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * EXAMPLE (c):
 * // ... //
 * int nsockets = 0;
 * if (QUO_SUCCESS != QUO_get_nobjs_by_type(q, QUO_OBJ_SOCKET, &nsockets)) {
 *     // error handling //
 * }
 */
int
QUO_get_nobjs_by_type(const QUO_t *q,
                      QUO_obj_type_t target_type,
                      int *out_nobjs);

/**
 * libquo context handle query routine that returns the total number of hardware
 * resource objects that are in another hardware resource.
 *
 * @param q - constructed and initialized QUO_t context pointer. (IN)
 *
 * @param in_type - container hardware object type. (IN)
 *
 * @param in_type_index - in_type's ID (base 0). (IN)
 *
 * @param type - total number of hardware objects found in
 *               in_type(in_type_index). (IN)
 *
 * @param out_nranks - total number of hardware object types found on the
 *                     system. (OUT)
 *
 * @param out_result- total number of hardware object types found by the query.
 *                    (OUT)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * EXAMPLE (c):
 * // ... //
 * int ncores_in_first_socket = 0;
 * if (QUO_SUCCESS != QUO_get_nobjs_in_type_by_type(q, QUO_OBJ_SOCKET, 0
 *                                                  QUO_OBJ_CORE,
 *                                                  &ncores_in_first_socket)) {
 *     // error handling //
 * }
 */
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

/**
 * returns the number of smp ranks (node ranks) whose current cpu binding policy
 * covers the given type and type index.
 */
int
QUO_nsmpranks_in_type(const QUO_t *q,
                      QUO_obj_type_t type,
                      int in_type_index,
                      int *n_out_smpranks);

/**
 * returns the number of smp ranks (node ranks) and an allocated array of the
 * MPI_COMM_WORLD rank ids whose current cpu binding policy covers the given
 * type and type index.
 */
int
QUO_smpranks_in_type(const QUO_t *q,
                     QUO_obj_type_t type,
                     int in_type_index,
                     int *n_out_smpranks,
                     int **out_smpranks);

int
QUO_nnumanodes(const QUO_t *q,
               int *out_nnumanodes);

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
QUO_nnodes(const QUO_t *q,
           int *out_nodes);

int
QUO_nnoderanks(const QUO_t *q,
               int *out_nnoderanks);

int
QUO_noderank(const QUO_t *q,
             int *out_noderank);

int
QUO_bound(const QUO_t *q,
          int *bound);

int
QUO_stringify_cbind(const QUO_t *q,
                    char **cbind_str);


int
QUO_bind_push(QUO_t *q,
              QUO_bind_push_policy_t policy,
              QUO_obj_type_t type,
              int obj_index);

int
QUO_bind_pop(QUO_t *q);

#ifdef __cplusplus
}
#endif

#endif
