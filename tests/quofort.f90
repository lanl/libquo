!
! Copyright (c) 2013      Los Alamos National Security, LLC.
!                         All rights reserved.
!

! my very first fortran app -- don't laugh too hard...

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
module QUO_MOD
    implicit none
    ! include quof and mpif
    include "quof.h"

CONTAINS

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! constructs and inits libquo context, quoc
subroutine QM_INIT(quoc)
    integer*8, intent(inout) :: quoc
    integer*4 :: qerr, initialized
    ! construct the quo context.
    call QUO_CONSTRUCT(quoc, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        print *, 'QUO_CONSTRUCT failure: err = ', qerr
        stop
    end if
    ! init the newly constructed context
    call QUO_INIT(quoc, qerr)
    if (QUO_SUCCESS .NE. qerr .AND. QUO_SUCCESS_ALREADY_DONE .NE. qerr) then
        print *, 'QUO_INIT failure: err = ', qerr
        stop
    end if
    ! exercise the interface by checking if the context is initialized.
    call QUO_INITIALIZED(quoc, initialized, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        print *, 'QUO_INITIALIZED failure: err = ', qerr
        stop
    end if
    ! how did this happen?
    if (initialized .NE. 1) then
        print *, '### libquo is not initialized. stopping'
    end if
    return
end subroutine QM_INIT

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! gathers basic system info
subroutine QM_SYSGROK(quoc, nnodes, nnoderanks, noderank, nsocks, ncores, npus)
    integer*8, intent(in) :: quoc
    integer*4, intent(out) :: nnodes, nnoderanks, noderank, nsocks, ncores, npus
    integer*4 :: qerr, tmpnsocks
    ! how many nodes are in our job
    call QUO_NNODES(quoc, nnodes, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        print *, 'QUO_NNODES failure: err = ', qerr
        stop
    end if
    ! what is my node rank (starts from 0)
    call QUO_NODERANK(quoc, noderank, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        print *, 'QUO_NODERANK failure: err = ', qerr
        stop
    end if
    ! how many ranks are on my node (includes myself)
    call QUO_NNODERANKS(quoc, nnoderanks, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        print *, 'QUO_NNODERANKS failure: err = ', qerr
        stop
    end if
    ! how many sockets are on this system
    call QUO_NSOCKETS(quoc, nsocks, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        print *, 'QUO_NSOCKETS failure: err = ', qerr
        stop
    end if
    ! how many cores are on this system
    call QUO_NCORES(quoc, ncores, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        print *, 'QUO_NCORES failure: err = ', qerr
        stop
    end if
    ! how many processing units (PUs) are on this system
    call QUO_NPUS(quoc, npus, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        print *, 'QUO_NPUS failure: err = ', qerr
        stop
    end if
    ! exercise the interface by checking if this routine works
    call QUO_GET_NOBJS_BY_TYPE(quoc, QUO_OBJ_SOCKET, tmpnsocks)
    if (QUO_SUCCESS .NE. qerr) then
        print *, 'QUO_GET_NOBJS_BY_TYPE failure: err = ', qerr
        stop
    end if
    ! make sure that this is consistent across the two calls
    if (tmpnsocks .NE. nsocks) then
        print *, 'QUO_GET_NOBJS_BY_TYPE broken. please report bug.'
        print *, 'nsocks = ', nsocks, 'tmpnsocks = ', tmpnsocks
        stop
    end if
    return
end subroutine QM_SYSGROK

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
subroutine QM_EMITBIND(quoc)
    integer*8, intent(in) :: quoc
    integer*4 :: bound, qerr
    ! is the process bound (cpu binding)
    call QUO_BOUND(quoc, bound, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        print *, 'QUO_BOUND failure: err = ', qerr
        stop
    end if
    if (1 .EQ. bound) then
        print *, '### process bound'
    else
        print *, '### process not bound'
    end if
    return
end subroutine QM_EMITBIND
end module QUO_MOD

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
program QUOFortF90
    use QUO_MOD

    implicit none

    include "mpif.h"

    ! holds the "quo context" that is passed around (must be the same size as
    ! the system's C pointer type.
    integer*8 :: quo
    ! holds libquo return codes, holds mpi return codes
    integer*4 qerr, ierr
    ! libquo uses standard ints. these sizes must be the same as the system's
    ! C int type.
    integer*4 :: quovmaj, quovmin, vindex
    integer*4 :: nnodes, nnoderanks, nsockets, ncores, npus, bound, noderank
    integer*4, allocatable, dimension(1) :: ranks(:)

    ! init mpi because quo needs it
    call MPI_INIT
    ! get libquo's version info
    call QUO_VERSION(quovmaj, quovmin, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        stop
    end if
    ! construct and init the quo context
    call QM_INIT(quo)

    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ! libquo is initialized, so we can get to work
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    ! gather basic system info
    call QM_SYSGROK(quo, nnodes, nnoderanks, noderank, nsockets, ncores, npus)
    ! allocate array large enough for node ranks
    allocate(ranks(nnoderanks))

    ! one rank per node will emit this info
    if (0 .EQ. noderank) then
        print *, '### quoversion: ', quovmaj, quovmin
        print *, '### nnodes    : ', nnodes
        print *, '### nnoderanks: ', nnoderanks
        print *, '### nsockets  : ', nsockets
        print *, '### ncores    : ', ncores
        print *, '### npus      : ', npus
    end if

    call QUO_BIND_PUSH(quo, QUO_BIND_PUSH_OBJ, QUO_OBJ_SOCKET, 0, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        print *, 'QUO_BIND_PUSH failure: err = ', qerr
        stop
    end if
    !call QM_EMITBIND(quo)
    ! note that the ranks array must be at least large enough to hold the
    ! result. also note that the ranks retured by this routine are
    ! MPI_COMM_WORLD ranks.
    call QUO_RANKS_ON_NODE(quo, ranks, qerr)
    do vindex=0, nnoderanks
        print *, ranks(vindex)
    end do
    ! finalize the quo context (always before MPI_FINALIZE)
    call QUO_FINALIZE(quo, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        print *, 'QUO_FINALIZE failure: err = ', qerr
        stop
    end if
    deallocate(ranks)
    ! finalize mpi (always after QUO_FINALIZE)
    call MPI_FINALIZE(ierr)
end program QUOFortF90
