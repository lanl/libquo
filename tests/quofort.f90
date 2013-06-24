!
! Copyright (c) 2013      Los Alamos National Security, LLC.
!                         All rights reserved.
!

! my very first fortran app -- don't laugh too hard...

program QUOFortF90
    implicit none
    ! include quof
    include "quof.h"
    include "mpif.h"
    ! holds the "quo context" that is passed around
    integer*8 :: quo
    ! holds libquo return codes
    integer*4 qerr
    integer*4 :: quovmaj, quovmin

    ! init mpi because quo needs it
    call MPI_INIT

    call QUO_VERSION(quovmaj, quovmin, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        stop
    end if
    print '("### ", A10, " = ", I4, I4)', 'quoversion', quovmaj, quovmin
    call QUO_CONSTRUCT(quo, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        print *, 'QUO_CONSTRUCT failure: err = ', qerr
        stop
    end if
    call QUO_INIT(quo, qerr)
    if (QUO_SUCCESS .NE. qerr .AND. QUO_SUCCESS_ALREADY_DONE .NE. qerr) then
        print *, 'QUO_INIT failure: err = ', qerr
        stop
    end if
end program QUOFortF90
