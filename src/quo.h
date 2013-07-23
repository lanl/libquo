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
/**
 * fatal = libquo can no longer function.
 * not fatal = libquo can continue functioning, but an error occurred.
 */
enum {
    /* success */
    QUO_SUCCESS = 0,
    /* success, but already done */
    QUO_SUCCESS_ALREADY_DONE,
    /* general error -- fatal */
    QUO_ERR,
    /* system error -- fatal */
    QUO_ERR_SYS,
    /* out of resources error  -- fatal */
    QUO_ERR_OOR,
    /* invalid argument provided to library -- usually fatal */
    QUO_ERR_INVLD_ARG,
    /* library call before QUO_init was called -- improper use of library */
    QUO_ERR_CALL_BEFORE_INIT,
    /* topology error -- fatal */
    QUO_ERR_TOPO,
    /* mpi error -- fatal */
    QUO_ERR_MPI,
    /* action not supported -- usually not fatal */
    QUO_ERR_NOT_SUPPORTED,
    /* *pop error -- not fatal, but usually indicates improper use */
    QUO_ERR_POP,
    /* the thing that you were looking for wasn't found -- not fatal */
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
    /* push the exact binding policy that was provided */
    QUO_BIND_PUSH_PROVIDED = 0,
    /* push to the "closest" QUO_obj_type_t provided */
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
 * process with the same active quo context.
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
 * libquo context query routine that returns the number of ranks that are on the
 * caller's node and their corresponding MPI_COMM_WORLD ranks. the number and
 * returned array also include the calling rank's information.
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
 * libquo query routine that returns a string that represents the system's
 * topology.
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
 * libquo context query routine that returns the total number of hardware
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
 * libquo context query routine that returns the total number of hardware
 * resource objects that are in another hardware resource.
 *
 * @param q - constructed and initialized QUO_t context pointer. (IN)
 *
 * @param in_type - container hardware object type. (IN)
 *
 * @param in_type_index - in_type's ID (base 0). (IN)
 *
 * @param type - total number of hardware objects found in
 *               in_type[in_type_index]. (IN)
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
 * libquo context handle query routine that returns whether or not my current
 * binding policy falls within a particular system hardware resource.
 *
 * @param q - constructed and initialized QUO_t context pointer. (IN)
 *
 * @param type - hardware object type. (IN)
 *
 * @param in_type_index - type's ID (base 0). (IN)
 *
 * @param out_result- flag indicating whether or not my current binding policy
 *                    falls within type[in_type_index]. (OUT)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * EXAMPLE (c):
 * // ... //
 * int cur_binding_in_third_socket = 0;
 * if (QUO_SUCCESS != QUO_cur_cpuset_in_type(q, QUO_OBJ_SOCKET, 2
 *                                           &cur_binding_in_third_socket)) {
 *     // error handling //
 * }
 * if (cur_binding_in_third_socket) {
 *     // do stuff //
 * }
 */
int
QUO_cur_cpuset_in_type(const QUO_t *q,
                       QUO_obj_type_t type,
                       int in_type_index,
                       int *out_result);

/**
 * libquo context query routine that returns the number of node ranks whose
 * current binding policy fall within a particular system hardware resource.
 *
 * @param q - constructed and initialized QUO_t context pointer. (IN)
 *
 * @param type - hardware object type. (IN)
 *
 * @param in_type_index - type's ID (base 0). (IN)
 *
 * @param n_out_smpranks - total number of node ranks that met the query
 *                         criteria. (OUT)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * EXAMPLE (c):
 * // ... //
 * int nnode_ranks_covering_socket0 = 0;
 * if (QUO_SUCCESS != QUO_nsmpranks_in_type(q, QUO_OBJ_SOCKET, 0
 *                                          &nnode_ranks_covering_socket0)) {
 *     // error handling //
 * }
 * if (nnode_ranks_covering_socket0 > 4) {
 *     // do stuff //
 * }
 */
int
QUO_nsmpranks_in_type(const QUO_t *q,
                      QUO_obj_type_t type,
                      int in_type_index,
                      int *n_out_smpranks);

/**
 * similar to QUO_nsmpranks_in_type, but also returns the "SMP_COMM_WORLD" ranks
 * that met the query criteria.
 *
 * @param q - constructed and initialized QUO_t context pointer. (IN)
 *
 * @param type - hardware object type. (IN)
 *
 * @param in_type_index - type's ID (base 0). (IN)
 *
 * @param n_out_smpranks - total number of node ranks that satisfy the query
 *                         criteria. (OUT)
 *
 * @param out_smpranks - an array of "SMP_COMM_WORLD" ranks that met the query
 *                       criteria. *out_smpranks must be freed by a call to
 *                       free(3). (OUT)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * EXAMPLE (c):
 * // ... //
 * int nnode_ranks_covering_socket0 = 0;
 * int *node_ranks_covering_socket0 = NULL;
 * if (QUO_SUCCESS != QUO_nsmpranks_in_type(q, QUO_OBJ_SOCKET, 0
 *                                          &nnode_ranks_covering_socket0,
 *                                          &node_ranks_covering_socket0)) {
 *     // error handling //
 * }
 * // ... //
 * free(node_ranks_covering_socket0);
 */
int
QUO_smpranks_in_type(const QUO_t *q,
                     QUO_obj_type_t type,
                     int in_type_index,
                     int *n_out_smpranks,
                     int **out_smpranks);

/**
 * libquo query routine that returns the total number of NUMA nodes that are
 * present on the caller's system.
 *
 * @param q - constructed and initialized QUO_t context pointer. (IN)
 *
 * @param out_nnumanodes - total number of NUMA nodes on the system. (OUT)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * EXAMPLE (c):
 * // ... //
 * int nnumanodes = 0;
 * if (QUO_SUCCESS != QUO_nnumanodes(q, &nnumanodes)) {_
 *     // error handling //
 * }
 * // ... //
 */
int
QUO_nnumanodes(const QUO_t *q,
               int *out_nnumanodes);

/**
 * similar to QUO_nnumanodes, but returns the total number of sockets present on
 * the caller's system.
 */
int
QUO_nsockets(const QUO_t *q,
             int *out_nsockets);

/**
 * similar to QUO_nnumanodes, but returns the total number of cores present on
 * the caller's system.
 */
int
QUO_ncores(const QUO_t *q,
           int *out_ncores);

/**
 * similar to QUO_nnumanodes, but returns the total number of processing units
 * (PUs) (e.g. hardware threads) present on the caller's system.
 */
int
QUO_npus(const QUO_t *q,
         int *out_npus);

/**
 * similar to QUO_nnumanodes, but returns the total number of compute nodes in
 * the current job.
 */
int
QUO_nnodes(const QUO_t *q,
           int *out_nodes);

/**
 * similar to QUO_nnumanodes, but returns the total number of ranks that are on
 * the caller's node.
 *
 * NOTES: *out_nnoderanks includes the caller. for example, if there are 3 MPI processes
 * on rank 0's node, then rank 0's call to this routine will result in
 * *out_nnoderanks being set to 3.
 */
int
QUO_nnoderanks(const QUO_t *q,
               int *out_nnoderanks);

/**
 * libquo query routine that returns the caller's "SMP_COMM_WORLD" rank.
 *
 * @param q - constructed and initialized QUO_t context pointer. (IN)
 *
 * @param out_noderank - the caller's node rank, as assigned by libquo. (OUT)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * NOTES: node ranks start at 0 and go to NNODERANKS - 1.
 *
 * EXAMPLE (c):
 * // ... //
 * int mynoderank = 0;
 * if (QUO_SUCCESS != QUO_noderank(q, &mynoderank)) {_
 *     // error handling //
 * }
 * if (0 == mynoderank) {
 *     // node rank 0 do stuff //
 * }
 */
int
QUO_noderank(const QUO_t *q,
             int *out_noderank);

/**
 * libquo query routine that returns whether or not the caller is currently
 * "bound" to a CPU resource.
 *
 * @param q - constructed and initialized QUO_t context pointer. (IN)
 *
 * @param bound - flag indicating whether or not the caller is currently bound.
 *                (OUT)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * NOTES: if the caller's current cpuset is equal to the widest available
 * cpuset, then the caller is not bound as far as libquo is concerned. for
 * example, if your system has only one core and the calling process is "bound"
 * to that one core, then as far as we are concerned, the caller is not bound.
 *
 * EXAMPLE (c):
 * // ... //
 * int bound = 0;
 * if (QUO_SUCCESS != QUO_bound(q, &bound)) {_
 *     // error handling //
 * }
 * if (!bound) {
 *     // take action //
 * }
 */

int
QUO_bound(const QUO_t *q,
          int *bound);

/**
 * libquo query routine that returns a string representation of the caller's
 * current binding policy (cpuset) in a hexadecimal format. @see CPUSET(7).
 *
 * @param q - constructed and initialized QUO_t context pointer. (IN)
 *
 * @param cbind_str - the caller's current CPU binding policy in string form.
 *                    *cbind_str must be freed by call to free(3). (OUT)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * EXAMPLE (c):
 * // ... //
 * char *cbindstr = NULL;
 * if (QUO_SUCCESS != QUO_stringify_cbind(q, &cbindstr)) {
 *     // error handling //
 * }
 * printf("%s\n", cbindstr);
 * free(cbindstr);
 */
int
QUO_stringify_cbind(const QUO_t *q,
                    char **cbind_str);


/**
 * libquo routine that changes the caller's process binding policy. the policy
 * is maintained in the current context's stack.
 *
 * @param q - constructed and initialized QUO_t context pointer. (IN)
 *
 * @param policy - policy that influence the behavior of this routine. if
 *                 QUO_BIND_PUSH_PROVIDED is provided, then the type and
 *                 obj_index are used as the new policy.  if QUO_BIND_PUSH_OBJ
 *                 is provided, then obj_index is ignored and the "closest" type
 *                 is used. (IN)
 *
 * @param type - the hardware resource to bind to. (IN)
 *
 * @param type - when not ignored -- type's index (base 0). (IN)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * NOTES: to revert to the previous binding policy call QUO_bind_pop.
 *
 * EXAMPLE 1 (c):
 * // ... //
 * // in this example we will bind to socket 0 //
 * if (QUO_SUCCESS != QUO_bind_push(q, QUO_BIND_PUSH_PROVIDED,
 *                                  QUO_OBJ_SOCKET, 0)) {
 *     // error handling //
 * }
 * // revert to previous process binding policy //
 * if (QUO_SUCCESS != QUO_bind_pop(q)) {
 *     // error handling //
 * }
 * // ... //
 *
 * EXAMPLE 2 (c):
 * // ... //
 * // in this example we will bind to the "closest" socket //
 * if (QUO_SUCCESS != QUO_bind_push(q, QUO_BIND_PUSH_OBJ,
 *                                  QUO_OBJ_SOCKET, -1)) {
 *     // error handling //
 * }
 * // revert to previous process binding policy //
 * if (QUO_SUCCESS != QUO_bind_pop(q)) {
 *     // error handling //
 * }
 * // ... //
 */
int
QUO_bind_push(QUO_t *q,
              QUO_bind_push_policy_t policy,
              QUO_obj_type_t type,
              int obj_index);

/**
 * libquo routine that changes the caller's process binding policy by replacing
 * it with the policy at the top of the provided context's process bind stack.
 *
 * @param q - constructed and initialized QUO_t context pointer. (IN)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * EXAMPLE (c):
 * // ... //
 * // in this example we will bind to socket 0 //
 * if (QUO_SUCCESS != QUO_bind_push(q, QUO_BIND_PUSH_PROVIDED,
 *                                  QUO_OBJ_SOCKET, 0)) {
 *     // error handling //
 * }
 * // revert to previous process binding policy //
 * if (QUO_SUCCESS != QUO_bind_pop(q)) {
 *     // error handling //
 * }
 * // ... //
 */
int
QUO_bind_pop(QUO_t *q);


int
QUO_node_barrier(QUO_t *q);

/**
 * TODO
 * don't forget to mention that this routine, as with others (identify), assume
 * that every process calls into this routine and that the info will be the same
 * across all calling processes. this isn't unreasonable, as we care about bulk
 * sync codes.
 *
 * TODO
 * document this thing.
 */

int
QUO_dist_work_member(const QUO_t *q,
                     QUO_obj_type_t distrib_over_this,
                     int max_members_per_res_type,
                     int *out_am_member);

#ifdef __cplusplus
}
#endif

#endif
