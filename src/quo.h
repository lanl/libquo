/**
 * Copyright (c) 2013      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * Copyright 2013. Los Alamos National Security, LLC. This software was produced
 * under U.S. Government contract DE-AC52-06NA25396 for Los Alamos National
 * Laboratory (LANL), which is operated by Los Alamos National Security, LLC for
 * the U.S. Department of Energy. The U.S. Government has rights to use,
 * reproduce, and distribute this software.  NEITHER THE GOVERNMENT NOR LOS
 * ALAMOS NATIONAL SECURITY, LLC MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR
 * ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE.  If software is modified
 * to produce derivative works, such modified software should be clearly marked,
 * so as not to confuse it with the version available from LANL.
 *
 * Additionally, redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following conditions
 * are met:
 *
 * · Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * · Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * · Neither the name of Los Alamos National Security, LLC, Los Alamos
 *   National Laboratory, LANL, the U.S. Government, nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS NATIONAL
 * SECURITY, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* i do a pretty terrible job explaining the interface. play around with the
 * demo codes; they are simple and pretty clearly illustrate how to use QUO. */

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
/* external QUO context type */
typedef QUO_t * QUO_context;

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
    /* push to the enclosing QUO_obj_type_t provided. */
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
 * libquo context handle construction and initialization routine.
 *
 * @param q - reference to a new QUO_context. (OUT)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * NOTES: this is typically the first "real" call into the library. a
 * relatively expensive routine that must be called AFTER MPI_Init. call
 * QUO_free to free returned resources.
 *
 * EXAMPLE (c):
 * QUO_context quo = NULL;
 * if (QUO_SUCCESS != QUO_create(&quo)) {
 *     // error handling //
 * }
 */

int
QUO_create(QUO_context *q);

/**
 * libquo context handle destruction routine.
 *
 * @param q - constructed and initialized QUO_context. (IN)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * NOTES: this is typically the last "real" call into the library.  a relatively
 * inexpensive routine that must be called BEFORE MPI_Finalize.  once a call to
 * this routine is made, it is an error to use any libquo services associated
 * with the freed libquo context from any other participating process.
 *
 * EXAMPLE (c):
 * // ... //
 * if (QUO_SUCCESS != QUO_free(quo)) {
 *     // error handling //
 * }
 */
int
QUO_free(QUO_context q);

/**
 * libquo context query routine that returns the total number of hardware
 * resource objects that are on the caller's system.
 *
 * @param q - constructed and initialized QUO_context. (IN)
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
 * if (QUO_SUCCESS != QUO_nobjs_by_type(q, QUO_OBJ_SOCKET, &nsockets)) {
 *     // error handling //
 * }
 */
int
QUO_nobjs_by_type(QUO_context q,
                  QUO_obj_type_t target_type,
                  int *out_nobjs);

/**
 * libquo context query routine that returns the total number of hardware
 * resource objects that are in another hardware resource (e.g. cores in a
 * socket).
 *
 * @param q - constructed and initialized QUO_context. (IN)
 *
 * @param in_type - container hardware object type. (IN)
 *
 * @param in_type_index - in_type's ID (base 0). (IN)
 *
 * @param type - target hardware object found in in_type[in_type_index]. (IN)
 *
 * @param out_result- total number of hardware object types found by the query.
 *                    (OUT)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * EXAMPLE (c):
 * // ... //
 * int ncores_in_first_socket = 0;
 * if (QUO_SUCCESS != QUO_nobjs_in_type_by_type(q, QUO_OBJ_SOCKET, 0
 *                                              QUO_OBJ_CORE,
 *                                              &ncores_in_first_socket)) {
 *     // error handling //
 * }
 */
int
QUO_nobjs_in_type_by_type(QUO_context q,
                          QUO_obj_type_t in_type,
                          int in_type_index,
                          QUO_obj_type_t type,
                          int *out_result);

/**
 * libquo context handle query routine that returns whether or not my current
 * binding policy falls within a particular system hardware resource (is
 * enclosed).
 *
 * @param q - constructed and initialized QUO_context. (IN)
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
 * int cur_bind_covers_sock3 = 0;
 * if (QUO_SUCCESS != QUO_cpuset_in_type(q, QUO_OBJ_SOCKET, 2
 *                                       &cur_bind_enclosed_in_sock3)) {
 *     // error handling //
 * }
 * if (cur_bind_enclosed_in_sock3) {
 *     // do stuff //
 * }
 */
int
QUO_cpuset_in_type(QUO_context q,
                   QUO_obj_type_t type,
                   int in_type_index,
                   int *out_result);

/**
 * similar to QUO_cpuset_in_type, but returns the "SMP_COMM_WORLD" QUO IDs that
 * met the query criteria.
 *
 * @param q - constructed and initialized QUO_context. (IN)
 *
 * @param type - hardware object type. (IN)
 *
 * @param in_type_index - type's ID (base 0). (IN)
 *
 * @param out_nqids - total number of node (job) processes that satisfy the
 *                    query criteria. (OUT)
 *
 * @param out_qids - an array of "SMP_COMM_WORLD ranks" that met the query
 *                   criteria. *out_qids must be freed by a call to
 *                   free(3). (OUT)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * EXAMPLE (c):
 * // ... //
 * int nqids_enclosed_in_socket0 = 0;
 * int *qids_enclosed_in_socket0 = NULL;
 * if (QUO_SUCCESS != QUO_qids_in_type(q, QUO_OBJ_SOCKET, 0
 *                                     &nqids_enclosed_in_socket0,
 *                                     &qids_enclosed_in_socket0)) {
 *     // error handling //
 * }
 * // ... //
 * free(qids_enclosed_in_socket0);
 */
int
QUO_qids_in_type(QUO_context q,
                 QUO_obj_type_t type,
                 int in_type_index,
                 int *out_nqids,
                 int **out_qids);

/**
 * libquo query routine that returns the total number of NUMA nodes that are
 * present on the caller's system.
 *
 * @param q - constructed and initialized QUO_context. (IN)
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
QUO_nnumanodes(QUO_context q,
               int *out_nnumanodes);

/**
 * similar to QUO_nnumanodes, but returns the total number of sockets present on
 * the caller's system.
 */
int
QUO_nsockets(QUO_context q,
             int *out_nsockets);

/**
 * similar to QUO_nnumanodes, but returns the total number of cores present on
 * the caller's system.
 */
int
QUO_ncores(QUO_context q,
           int *out_ncores);

/**
 * similar to QUO_nnumanodes, but returns the total number of processing units
 * (PUs) (e.g. hardware threads) present on the caller's system.
 */
int
QUO_npus(QUO_context q,
         int *out_npus);

/**
 * similar to QUO_nnumanodes, but returns the total number of compute nodes
 * (i.e. servers) in the current job.
 */
int
QUO_nnodes(QUO_context q,
           int *out_nodes);

/**
 * similar to QUO_nnumanodes, but returns the total number of job processes that
 * are on the caller's compute node.
 *
 * NOTES: *out_nqids includes the caller. for example, if there are 3 MPI
 * processes on rank 0's (MPI_COMM_WORLD) node, then rank 0's call to this
 * routine will result in *out_nqids being set to 3.
 */
int
QUO_nqids(QUO_context q,
          int *out_nqids);

/**
 * libquo query routine that returns the caller's compute node QUO node ID.
 *
 * @param q - constructed and initialized QUO_context. (IN)
 *
 * @param out_qid - the caller's node ID, as assigned by libquo. (OUT)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * NOTES: QIDs start at 0 and go to NNODERANKS - 1.
 *
 * EXAMPLE (c):
 * // ... //
 * int mynodeqid = 0;
 * if (QUO_SUCCESS != QUO_id(q, &mynodeqid)) {_
 *     // error handling //
 * }
 * if (0 == mynodeqid) {
 *     // node id 0 do stuff //
 * }
 */
int
QUO_id(QUO_context q,
       int *out_qid);

/**
 * libquo query routine that returns whether or not the caller is currently
 * "bound" to a CPU resource.
 *
 * @param q - constructed and initialized QUO_context. (IN)
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
QUO_bound(QUO_context q,
          int *bound);

/**
 * libquo query routine that returns a string representation of the caller's
 * current binding policy (cpuset) in a hexadecimal format. @see CPUSET(7).
 *
 * @param q - constructed and initialized QUO_context. (IN)
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
QUO_stringify_cbind(QUO_context q,
                    char **cbind_str);


/**
 * libquo routine that changes the caller's process binding policy. the policy
 * is maintained in the current context's stack.
 *
 * @param q - constructed and initialized QUO_context. (IN)
 *
 * @param policy - policy that influence the behavior of this routine. if
 *                 QUO_BIND_PUSH_PROVIDED is provided, then the type and
 *                 obj_index are used as the new policy.  if QUO_BIND_PUSH_OBJ
 *                 is provided, then obj_index is ignored and the "closest" type
 *                 is used. (IN)
 *
 * @param type - the hardware resource to bind to. (IN)
 *
 * @param obj_index - when not ignored, type's index (base 0). (IN)
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
QUO_bind_push(QUO_context q,
              QUO_bind_push_policy_t policy,
              QUO_obj_type_t type,
              int obj_index);

/**
 * libquo routine that changes the caller's process binding policy by replacing
 * it with the policy at the top of the provided context's process bind stack.
 *
 * @param q - constructed and initialized QUO_context. (IN)
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
QUO_bind_pop(QUO_context q);

/**
 * libquo routine that acts as a compute node barrier. all processes on a node
 * MUST call this in order for everyone to proceed past the barrier on a single
 * compute node. see demos for examples.
 *
 * @param q - constructed and initialized QUO_context. (IN)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * EXAMPLE (c): // ... //
 *  // time for p1 to do some work with some of the ranks //
 *  if (working) {
 *      // *** do work *** //
 *      // signals completion //
 *      if (QUO_SUCCESS != QUO_barrier(q)) {
 *          // error handling //
 *      }
 *  } else {
 *      // non workers wait in a barrier //
 *      if (QUO_SUCCESS != QUO_barrier(q)) {
 *          // error handling //
 *      }
 *  }
 *  // ... ///
 */
int
QUO_barrier(QUO_context q);

/**
 * libquo routine that helps evenly distribute processes across hardware
 * resources.  the total number of processes assigned to a particular resource
 * will not exceed max_qids_per_res_type.
 *
 * @param q - constructed and initialized QUO_context. (IN)
 *
 * @param distrib_over_this - the target hardware resource on which processes
 *                            will be evenly distributed.
 *
 * @param max_qids_per_res_type - the maximum number of processes that will be
 *                                assigned to the provided resources. for
 *                                example, if your system has two sockets and
 *                                max_qids_per_res_type is 2, then a max of 4
 *                                processes will be chosen (max 2 per socket).
 *                                this routine doesn't modify the calling
 *                                processes' affinities, but is used as a
 *                                helper for evenly distributing processes over
 *                                hardware resources given a global view of all
 *                                the affinities within a job. i'm doing a
 *                                terrible job explaining this, so look at the
 *                                demos. believe me, this routine is useful...
 *
 * @param out_selected - flag indicating whether or not i was chosen in the work
 *                       distribution. 1 means I was chosen, 0 otherwise. (OUT)
 *
 * @returnvalue QUO_SUCCESS if the operation completed successfully.
 *
 * EXAMPLE (c):
 * // ... //
 * int res_assigned = 0;
 * if (QUO_SUCCESS != QUO_auto_distrib(q, QUO_OBJ_SOCKET,
 *                                     2, &res_assigned)) {
 *     // error handling //
 * }
 * // ... //
 */
int
QUO_auto_distrib(QUO_context q,
                 QUO_obj_type_t distrib_over_this,
                 int max_qids_per_res_type,
                 int *out_selected);

#ifdef __cplusplus
}
#endif

#endif

int QUO_bind_threads(QUO_context q, QUO_obj_type_t type, int index);
