!
! Copyright (c) 2013      Los Alamos National Security, LLC.
!                         All rights reserved.
!

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!! update if quo.h ever changes !!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! return codes
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
    parameter (QUO_SUCCESS_ALREADY_DONE = 1)
    parameter (QUO_ERR = 2)
    parameter (QUO_ERR_SYS = 3)
    parameter (QUO_ERR_OOR = 4)
    parameter (QUO_ERR_INVLD_ARG = 5)
    parameter (QUO_ERR_CALL_BEFORE_INIT = 6)
    parameter (QUO_ERR_TOPO = 7)
    parameter (QUO_ERR_MPI = 8)
    parameter (QUO_ERR_NOT_SUPPORTED = 9)
    parameter (QUO_ERR_POP = 10)
    parameter (QUO_ERR_NOT_FOUND = 11)

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! quo object types
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    integer QUO_OBJ_MACHINE
    integer QUO_OBJ_NODE
    integer QUO_OBJ_SOCKET
    integer QUO_OBJ_CORE
    integer QUO_OBJ_PU

    parameter (QUO_OBJ_MACHINE = 0)
    parameter (QUO_OBJ_NODE = 1)
    parameter (QUO_OBJ_SOCKET = 2)
    parameter (QUO_OBJ_CORE = 3)
    parameter (QUO_OBJ_PU = 4)

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! push policies
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    integer QUO_BIND_PUSH_PROVIDED
    integer QUO_BIND_PUSH_OBJ

    parameter (QUO_BIND_PUSH_PROVIDED = 0)
    parameter (QUO_BIND_PUSH_OBJ = 1)
