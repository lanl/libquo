!
! Copyright (c) 2013      Los Alamos National Security, LLC.
!                         All rights reserved.
!

! my very first fortran app -- don't laugh too hard...

program QUOFortF90
    implicit none
    ! include quof
    include "quof.h"
    ! holds libquo return codes
    integer ( kind = 4 ) qerr
    integer ( kind = 4 ) :: quovmaj, quovmin
    call QUO_VERSION(quovmaj, quovmin, qerr)
    if (QUO_SUCCESS .NE. qerr) then
        stop
    end if
    print '("### ", A10, " = ", I4, I4)', 'quoversion', quovmaj, quovmin
end program QUOFortF90
