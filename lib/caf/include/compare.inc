!GCC$ ATTRIBUTES inline :: compare_strings_scalar
pure integer function compare_strings_scalar(data, i, j, len) result(is_eq)
    use iso_c_binding
    use iso_fortran_env
    implicit none

    integer(c_int8_t), intent(in) :: data(:)
    integer(8), intent(in) :: i, j
    integer(c_int32_t), intent(in) :: len
    integer(8) :: k
    
    is_eq = 1
    do k = 0, len - 1
        if (data(i + k) /= data(j + k)) then
            is_eq = 0
            exit
        end if
    end do
end function compare_strings_scalar
