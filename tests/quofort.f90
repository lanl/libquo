!
! Copyright (c) 2013      Los Alamos National Security, LLC.
!                         All rights reserved.
!

! my very first fortran app -- don't laugh too hard...

! does nothing useful. just used to exercise the fortran interface.
! for a better example, please see: quodemo-p0.c and quodemo-p1.c

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
    call QUO_GET_NOBJS_BY_TYPE(quoc, QUO_OBJ_SOCKET, tmpnsocks, qerr)
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
    integer*4 :: rank, nranks, coverflag
    integer*4 :: quovmaj, quovmin, vindex, ncoresinfsock, nsmpranksonfsock
    integer*4 :: nnodes, nnoderanks, nsockets, ncores, npus, bound, noderank
    integer*4, allocatable, dimension(1) :: ranks(:), smpranksonfsock(:)
    character(LEN=32) :: strbindprefix
    character(:), allocatable :: cstrbindprefix

    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ! mpi stuff
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ! init mpi because quo needs it
    call MPI_INIT(ierr)
    if (MPI_SUCCESS .NE. ierr) then
        print *, 'MPI_INIT failure: err = ', qerr
        stop
    end if
    call MPI_COMM_SIZE(MPI_COMM_WORLD, nranks, ierr)
    if (MPI_SUCCESS .NE. ierr) then
        print *, 'MPI_COMM_SIZE failure: err = ', qerr
        stop
    end if
    call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierr)
    if (MPI_SUCCESS .NE. ierr) then
        print *, 'MPI_COMM_RANK failure: err = ', qerr
        stop
    end if

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
        print *
        print *, '#############################################################'
        print *, '### nranks:             ', nranks
        print *, '### quoversion:         ', quovmaj, quovmin
        print *, '### nnodes:             ', nnodes
        print *, '### nnoderanks:         ', nnoderanks
        print *, '### nsockets:           ', nsockets
        print *, '### ncores:             ', ncores
        print *, '### npus:               ', npus
        print *, '#############################################################'
        print *
    end if

    ! setup bind emit prefix
    write(strbindprefix, '(I8)') rank
    ! play nice with C strings
    cstrbindprefix = '### rank: ' // adjustl(strbindprefix) // CHAR(0)
    ! note that the ranks array must be at least large enough to hold the
    ! result. also note that the ranks retured by this routine are
    ! MPI_COMM_WORLD ranks.
    call QUO_RANKS_ON_NODE(quo, ranks, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        print *, 'QUO_RANKS_ON_NODE failure: err = ', qerr
        stop
    end if
    ! print node ranks
    if (0 .EQ. noderank) then
        print *, '### MPI_COMM_WORLD node ranks'
        do vindex = 1, nnoderanks
            print *, ranks(vindex)
        end do
        print *, '### end MPI_COMM_WORLD node ranks'
    end if
    ! returns the number of objects in a particular type. for example: give me
    ! the number of cores in socket 0.
    call QUO_GET_NOBJS_IN_TYPE_BY_TYPE(quo, QUO_OBJ_SOCKET, 0, &
                                       QUO_OBJ_CORE, ncoresinfsock, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        print *, 'QUO_GET_NOBJS_IN_TYPE_BY_TYPE failure: err = ', qerr
        stop
    end if
    ! returns the number of node ranks whose current cpu binding policy covers a
    ! particular hardware resource. for example: the number of node ranks that
    ! are currently bound to socket 0.
    call QUO_NSMPRANKS_IN_TYPE(quo, QUO_OBJ_SOCKET, 0, nsmpranksonfsock, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        print *, 'QUO_NSMPRANKS_IN_TYPE failure: err = ', qerr
        stop
    end if
    ! now allocate the array so we can get the ranks
    allocate(smpranksonfsock(nsmpranksonfsock))
    ! now that the storage for the ranks array has been allocated, now populate
    ! it with the MPI_COMM_WORLD ranks that are currently bound to socket 0
    call QUO_SMPRANKS_IN_TYPE(quo, QUO_OBJ_SOCKET, 0, smpranksonfsock, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        print *, 'QUO_SMPRANKS_IN_TYPE failure: err = ', qerr
        stop
    end if
    call QUO_CUR_CPUSET_IN_TYPE(quo, QUO_OBJ_CORE, 0, coverflag, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        print *, 'QUO_CUR_CPUSET_IN_TYPE failure: err = ', qerr
        stop
    end if
    ! echo some more info
    if (0 .EQ. noderank) then
        print *, '### ncores in socket 0: ', ncoresinfsock
        print *, '### nsmpranks covering socket 0: ', nsmpranksonfsock
        print *, '### MPI_COMM_WORLD node ranks covering socket 0'
        do vindex = 1, nsmpranksonfsock
            print *, smpranksonfsock(vindex)
        end do
        print *, '### end MPI_COMM_WORLD node ranks covering socket 0'
    end if
    ! now everyone that covers core 0 be happy and print stuff
    if (1 .EQ. coverflag) then
        print *, '### rank covers core 0: ', rank
    end if
    !call QM_EMITBIND(quo)

    call MPI_BARRIER(MPI_COMM_WORLD, ierr)
    call SLEEP(1)

    if (0 .EQ. noderank) then
        print *
        print *, '!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!'
        print *, '!!! state of the bind !!!'
        print *, '!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!'
        print *
    end if
    call QUO_EMIT_CBIND_STRING(quo, cstrbindprefix, qerr)

    call MPI_BARRIER(MPI_COMM_WORLD, ierr)
    call SLEEP(1)

    if (0 .EQ. noderank) then
        print *
        print *, '!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!'
        print *, '!!! pushing new bind policy !!!'
        print *, '!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!'
        print *
    end if
    ! bind everyone to socket 0
    call QUO_BIND_PUSH(quo, QUO_BIND_PUSH_OBJ, QUO_OBJ_SOCKET, 0, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        print *, 'QUO_BIND_PUSH failure: err = ', qerr
        stop
    end if
    call QUO_EMIT_CBIND_STRING(quo, cstrbindprefix, qerr)

    call MPI_BARRIER(MPI_COMM_WORLD, ierr)
    call SLEEP(1)

    if (0 .EQ. noderank) then
        print *
        print *, '!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!'
        print *, '!!! reverting bind policy !!!'
        print *, '!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!'
        print *
    end if
    ! revert binding policy
    call QUO_BIND_POP(quo, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        print *, 'QUO_BIND_POP failure: err = ', qerr
        stop
    end if
    call QUO_EMIT_CBIND_STRING(quo, cstrbindprefix, qerr)
    call MPI_BARRIER(MPI_COMM_WORLD, ierr)
    call SLEEP(1)

    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ! cleanup
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ! finalize the quo context (always before MPI_FINALIZE)
    call QUO_FINALIZE(quo, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        print *, 'QUO_FINALIZE failure: err = ', qerr
        stop
    end if
    call QUO_DESTRUCT(quo, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        print *, 'QUO_DESTRUCT failure: err = ', qerr
        stop
    end if
    deallocate(ranks)
    deallocate(smpranksonfsock)
    ! finalize mpi (always after QUO_FINALIZE)
    call MPI_FINALIZE(ierr)
end program QUOFortF90
