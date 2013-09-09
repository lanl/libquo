!
! Copyright (c) 2013      Los Alamos National Security, LLC.
!                         All rights reserved.
!

module quo

      use quo_types

interface
      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      integer(c_int) &
      function quo_version_c(version, subversion) &
          bind(c, name='QUO_version')
          use, intrinsic :: iso_c_binding, only: c_int
          implicit none
          integer(c_int), intent(out) :: version, subversion
      end function quo_version_c

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      integer(c_int) &
      function quo_create_c(q) &
          bind(c, name='QUO_create')
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), intent(out) :: q
      end function quo_create_c

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      integer(c_int) &
      function quo_free_c(q) &
          bind(c, name='QUO_free')
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
      end function quo_free_c

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      integer(c_int) &
      function quo_nobjs_by_type_c(q, target_type, out_nobjs) &
          bind(c, name='QUO_nobjs_by_type')
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), value :: target_type
          integer(c_int), intent(out) :: out_nobjs
      end function quo_nobjs_by_type_c 

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      integer(c_int) &
      function quo_nobjs_in_type_by_type_c(q, in_type, type_index, &
                                           obj_type, oresult) &
          bind(c, name='QUO_nobjs_in_type_by_type')
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), value :: in_type, type_index, obj_type
          integer(c_int), intent(out) :: oresult 
      end function quo_nobjs_in_type_by_type_c

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      integer(c_int) &
      function quo_cpuset_in_type_c(q, obj_type, type_index, oresult) &
          bind(c, name='QUO_cpuset_in_type')
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), value :: obj_type, type_index
          integer(c_int), intent(out) :: oresult 
      end function quo_cpuset_in_type_c

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      integer(c_int) &
      function quo_qids_in_type_c(q, obj_type, type_index, &
                                  onqids, qids) &
          bind(c, name='QUO_qids_in_type')
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), value :: obj_type, type_index
          integer(c_int), intent(out) :: onqids
          type(c_ptr), intent(out) :: qids
      end function quo_qids_in_type_c 

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      integer(c_int) &
      function quo_nnumanodes_c(q, n) &
          bind(c, name='QUO_nnumanodes')
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), intent(out) :: n
      end function quo_nnumanodes_c

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      integer(c_int) &
      function quo_nsockets_c(q, n) &
          bind(c, name='QUO_nsockets')
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), intent(out) :: n
      end function quo_nsockets_c

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      integer(c_int) &
      function quo_ncores_c(q, n) &
          bind(c, name='QUO_ncores')
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), intent(out) :: n
      end function quo_ncores_c

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      integer(c_int) &
      function quo_npus_c(q, n) &
          bind(c, name='QUO_npus')
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), intent(out) :: n
      end function quo_npus_c

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      integer(c_int) &
      function quo_nnodes_c(q, n) &
          bind(c, name='QUO_nnodes')
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), intent(out) :: n
      end function quo_nnodes_c

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      integer(c_int) &
      function quo_nqids_c(q, n) &
          bind(c, name='QUO_nqids')
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), intent(out) :: n
      end function quo_nqids_c

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      integer(c_int) &
      function quo_id_c(q, n) &
          bind(c, name='QUO_id')
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), intent(out) :: n
      end function quo_id_c

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      integer(c_int) &
      function quo_bound_c(q, bound) &
          bind(c, name='QUO_bound')
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), intent (out) :: bound
      end function quo_bound_c

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      integer(c_int) &
      function quo_bind_push_c(q, policy, obj_type, obj_index) &
          bind(c, name='QUO_bind_push')
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), value :: policy, obj_type, obj_index
      end function quo_bind_push_c

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      integer(c_int) &
      function quo_bind_pop_c(q) &
          bind(c, name='QUO_bind_pop')
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
      end function quo_bind_pop_c

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      integer(c_int) &
      function quo_barrier_c(q) &
          bind(c, name='QUO_barrier')
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
      end function quo_barrier_c

end interface

contains
      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      subroutine quo_version(version, subversion, ierr)
          use, intrinsic :: iso_c_binding, only: c_int
          implicit none
          integer(c_int), intent(out) :: version, subversion, ierr
          ierr = quo_version_c(version, subversion)
      end subroutine quo_version

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      subroutine quo_create(q, ierr)
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), intent(out) :: q
          integer(c_int), intent(out) :: ierr
          ierr = quo_create_c(q)
      end subroutine quo_create

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      subroutine quo_free(q, ierr)
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), intent(out) :: ierr
          ierr = quo_free_c(q)
      end subroutine quo_free

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      subroutine quo_nobjs_by_type(q, target_type, out_nobjs, ierr)
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), value :: target_type
          integer(c_int), intent(out) :: out_nobjs
          integer(c_int), intent(out) :: ierr
          ierr = quo_nobjs_by_type_c(q, target_type, out_nobjs)
      end subroutine quo_nobjs_by_type

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      subroutine quo_nobjs_in_type_by_type(q, in_type, type_index, &
                                           obj_type, oresult, ierr)
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), value :: in_type, type_index, obj_type
          integer(c_int), intent(out) :: oresult, ierr
          ierr = quo_nobjs_in_type_by_type_c(q, in_type, type_index, &
                                             obj_type, oresult)
      end subroutine quo_nobjs_in_type_by_type

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      subroutine quo_cpuset_in_type(q, obj_type, type_index, &
                                    oresult, ierr)
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), value :: obj_type, type_index
          logical, intent (out) :: oresult
          integer(c_int), intent(out) :: ierr
          integer(c_int) :: ires
          ierr = quo_cpuset_in_type_c(q, obj_type, type_index, ires)
          oresult = (ires == 1)
      end subroutine quo_cpuset_in_type

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      subroutine quo_qids_in_type(q, obj_type, type_index, qids, ierr)
          use, intrinsic :: iso_c_binding
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), value :: obj_type, type_index
          integer(c_int), allocatable, intent(inout) :: qids(:)
          integer(c_int), pointer :: qidsp(:)
          type(c_ptr) :: qidp
          integer(c_int), intent(out) :: ierr
          integer(c_int) :: nqids, i
          ierr = quo_qids_in_type_c(q, obj_type, type_index, &
                                    nqids, qidp)
          call c_f_pointer(qidp, qidsp, [nqids])
          allocate (qids(nqids))
          forall (i = 1 : size(qidsp)) qids(i) = qidsp(i)
          ! XXX add free
      end subroutine quo_qids_in_type

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      subroutine quo_nnumanodes(q, n, ierr)
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), intent(out) :: n, ierr
          ierr = quo_nnumanodes_c(q, n)
      end subroutine quo_nnumanodes

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      subroutine quo_nsockets(q, n, ierr)
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), intent(out) :: n, ierr
          ierr = quo_nsockets_c(q, n)
      end subroutine quo_nsockets

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      subroutine quo_ncores(q, n, ierr)
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), intent(out) :: n, ierr
          ierr = quo_ncores_c(q, n)
      end subroutine quo_ncores

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      subroutine quo_npus(q, n, ierr)
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), intent(out) :: n, ierr
          ierr = quo_npus_c(q, n)
      end subroutine quo_npus

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      subroutine quo_nnodes(q, n, ierr)
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), intent(out) :: n, ierr
          ierr = quo_nnodes_c(q, n)
      end subroutine quo_nnodes

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      subroutine quo_nqids(q, n, ierr)
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), intent(out) :: n, ierr
          ierr = quo_nqids_c(q, n)
      end subroutine quo_nqids

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      subroutine quo_id(q, n, ierr)
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), intent(out) :: n, ierr
          ierr = quo_id_c(q, n)
      end subroutine quo_id

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      subroutine quo_bound(q, bound, ierr)
          use, intrinsic :: iso_c_binding
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          logical, intent (out) :: bound
          integer(c_int), intent(out) :: ierr
          integer(c_int) :: ibound = 0
          ierr = quo_bound_c(q, ibound)
          bound = (ibound == 1)
      end subroutine quo_bound

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      subroutine quo_bind_push(q, policy, obj_type, obj_index, ierr)
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), value :: policy, obj_type, obj_index
          integer(c_int), intent(out) :: ierr
          ierr = quo_bind_push_c(q, policy, obj_type, obj_index)
      end subroutine quo_bind_push

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      subroutine quo_bind_pop(q, ierr)
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), intent(out) :: ierr
          ierr = quo_bind_pop_c(q)
      end subroutine quo_bind_pop

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      subroutine quo_barrier(q, ierr)
          use, intrinsic :: iso_c_binding, only: c_int
          use :: quo_types
          implicit none
          type(quo_context), value :: q
          integer(c_int), intent(out) :: ierr
          ierr = quo_barrier_c(q)
      end subroutine quo_barrier

end module quo
