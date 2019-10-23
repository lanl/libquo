!
! Copyright (c) 2013-2019 Triad National Security, LLC
!                         All rights reserved.
!
! This file is part of the libquo project. See the LICENSE file at the
! top-level directory of this distribution.
!

! does nothing useful. just used to exercise the fortran interface.
! better examples can be found in demos

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
program quofort

    use quo
    use, intrinsic :: iso_c_binding
    implicit none

    include "mpif.h"

    logical bound, inres, have_res
    integer(c_int) info
    integer(c_int) ver, subver
    integer(c_int) nres, qid
    integer(c_int) cwrank
    integer(c_int), allocatable, dimension(:) :: sock_qids
    type(c_ptr) quoc
    integer machine_comm

    call quo_version(ver, subver, info)

    print *, info, ver, subver

    call mpi_init(info)
    call mpi_comm_rank(MPI_COMM_WORLD, cwrank, info)

    call quo_create(quoc, MPI_COMM_WORLD, info)

    call quo_bound(quoc, bound, info)

    call quo_nobjs_by_type(quoc, QUO_OBJ_CORE, nres, info)
    print *, 'bound, nres', bound, nres

    call quo_nobjs_in_type_by_type(quoc, QUO_OBJ_MACHINE, 0, &
                                   QUO_OBJ_SOCKET, nres, info)
    print *,'sock on machine', nres

    call quo_cpuset_in_type(quoc, QUO_OBJ_SOCKET, 0, inres, info)
    print *, 'rank on sock 0', cwrank, inres

    call quo_qids_in_type(quoc, QUO_OBJ_SOCKET, 0, sock_qids, info)
    print *, 'sock_qids', sock_qids
    deallocate (sock_qids)

    ! The output should be identical to QUO_OBJ_SOCKET query.
    call quo_qids_in_type(quoc, QUO_OBJ_PACKAGE, 0, sock_qids, info)
    print *, 'package_qids', sock_qids
    deallocate (sock_qids)

    call quo_nnumanodes(quoc, nres, info)
    print *, 'nnumanodes', nres

    call quo_nsockets(quoc, nres, info)
    print *, 'nsockets', nres

    call quo_ncores(quoc, nres, info)
    print *, 'ncores', nres

    call quo_npus(quoc, nres, info)
    print *, 'npus', nres

    call quo_nnodes(quoc, nres, info)
    print *, 'nnodes', nres

    call quo_nqids(quoc, nres, info)
    print *, 'nqids', nres

    call quo_id(quoc, qid, info)
    print *, 'qid', qid

    if (qid == 0) then
        print *, 'hello from qid 0!'
    endif

    call quo_bind_push(quoc, QUO_BIND_PUSH_OBJ, QUO_OBJ_SOCKET, -1, info)

    call quo_bound(quoc, bound, info)
    print *, 'bound after push', bound

    call quo_bind_pop(quoc, info)

    call quo_bound(quoc, bound, info)
    print *, 'bound after pop', bound

    call quo_auto_distrib(quoc, QUO_OBJ_SOCKET, 2, have_res, info)
    print *, 'rank, have_res', cwrank, have_res

    call quo_barrier(quoc, info)

    call quo_get_mpi_comm_by_type(quoc, QUO_OBJ_MACHINE, machine_comm, info)
    if (info /= QUO_SUCCESS) then
        print *, 'QUO_FAILURE DETECTED, info', info
        stop
    end if

    call mpi_comm_free(machine_comm, info)

    call quo_free(quoc, info)

    call mpi_finalize(info)

end program quofort
