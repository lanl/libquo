!
! Copyright (c) 2013      Los Alamos National Security, LLC.
!                         All rights reserved.
!

module quo_types

      use, intrinsic :: iso_c_binding

      type, bind(c) :: quo_context
          type(c_ptr) :: qcp
      end type quo_context

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      ! return codes
      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      integer(c_int) QUO_SUCCESS
      integer(c_int) QUO_SUCCESS_ALREADY_DONE
      integer(c_int) QUO_ERR
      integer(c_int) QUO_ERR_SYS
      integer(c_int) QUO_ERR_OOR
      integer(c_int) QUO_ERR_INVLD_ARG
      integer(c_int) QUO_ERR_CALL_BEFORE_INIT
      integer(c_int) QUO_ERR_TOPO
      integer(c_int) QUO_ERR_MPI
      integer(c_int) QUO_ERR_NOT_SUPPORTED
      integer(c_int) QUO_ERR_POP
      integer(c_int) QUO_ERR_NOT_FOUND

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

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      ! quo object types 
      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      integer(c_int) QUO_OBJ_MACHINE
      integer(c_int) QUO_OBJ_NODE
      integer(c_int) QUO_OBJ_SOCKET
      integer(c_int) QUO_OBJ_CORE
      integer(c_int) QUO_OBJ_PU

      parameter (QUO_OBJ_MACHINE = 0)
      parameter (QUO_OBJ_NODE = 1)
      parameter (QUO_OBJ_SOCKET = 2)
      parameter (QUO_OBJ_CORE = 3)
      parameter (QUO_OBJ_PU = 4)

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      ! push policies
      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      integer(c_int) QUO_BIND_PUSH_PROVIDED
      integer(c_int) QUO_BIND_PUSH_OBJ

      parameter (QUO_BIND_PUSH_PROVIDED = 0)
      parameter (QUO_BIND_PUSH_OBJ = 1)

end module quo_types
