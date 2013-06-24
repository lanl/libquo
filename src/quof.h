!
! Copyright (c) 2013      Los Alamos National Security, LLC.
!                         All rights reserved.
!

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! update if the C interface ever changes
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    integer QUO_SUCCESS
    integer QUO_SUCCESS_ALREADY_DONE
    integer QUO_ERR
    integer QUO_ERR_SYS
    integer QUO_ERR_OOR
    integer QUO_ERR_INVLD_ARG
    integer QUO_ERR_CALL_BEFORE_INIT
    integer QUO_ERR_TOPO
    integer QUO_ERR_MPI
    integer QUO_ERR_NOT_SUPPORTED
    integer QUO_ERR_POP
    integer QUO_ERR_NOT_FOUND

    parameter(QUO_SUCCESS = 0)

    integer QUO_OBJ_MACHINE
    integer QUO_OBJ_NODE
    integer QUO_OBJ_SOCKET
    integer QUO_OBJ_CORE
    integer QUO_OBJ_PU

    integer QUO_BIND_PUSH_PROVIDED
    integer QUO_BIND_PUSH_OBJ
