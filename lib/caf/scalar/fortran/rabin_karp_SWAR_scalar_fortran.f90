program rabin_karp_SWAR_scalar_fortran
    use iso_c_binding
    use iso_fortran_env
    implicit none
    
    character(len=256) :: input_file, output_file
    character(len=20) :: len_str
    integer(c_int32_t) :: len
    integer :: ios
    
    integer(c_int8_t), allocatable :: data(:)
    integer(c_int32_t), allocatable :: freq(:)[:]
    integer(c_size_t) :: size
    real(c_double) :: time
    
    integer(8) :: i, num_imgs, this_img, block_size
    integer(8) :: alloc_size
    integer(8) :: block_start, block_end
    integer(1) :: write_file_flag
    
    flush(6)
    
    if (command_argument_count() < 1) then
        stop 'Usage: program <input_file>'
    end if
    call get_command_argument(1, input_file)
    
    len = 128
    if (command_argument_count() >= 2) then
        call get_command_argument(2, len_str)
        read(len_str, *, iostat=ios) len
        if (ios /= 0) stop 'Invalid length argument'
    endif
    
    write_file_flag = 0
    if (command_argument_count() >= 3) then
        write_file_flag = 1
        call get_command_argument(3, output_file)        
    end if
    
    call read_input_file(input_file, data, size)

    alloc_size = int(size, kind=8) + 4097    
    num_imgs = num_images()
    this_img = this_image()
    
    block_size = (size - len + num_imgs) / num_imgs
    block_start = (this_img - 1)*block_size + 1
    block_end = min(this_img * block_size, int(size - len + 1, kind=8))
    
    allocate(freq(alloc_size)[*])
    do i = block_start, block_end, 1
        freq(i) = 0
    end do
    sync all
    
    call run_fortran_kernel(data, freq, len, size, time, block_start, block_end)
    
    if (write_file_flag > 0) then
        call write_result_file(output_file, freq, size, time, block_size)
    else
        if (this_img == 1) then
            write(*, '(A,I0,A,I0,A,F10.8,A)') &
                'text size=', size,    &
                ', pattern size=', len, &
                ', time=', time, ' s'
        end if
    end if
    
    flush(6)
    deallocate(data)
    deallocate(freq)
    
contains

    include 'compare.inc'

    subroutine kernel(i, data, freq, len, size)
        use iso_c_binding
        implicit none
        
        integer(8), intent(in) :: i
        integer(c_int8_t), intent(in) :: data(:)
        integer(c_int32_t), allocatable, intent(inout) :: freq(:)[:]
        integer(c_int32_t), intent(in) :: len
        integer(c_size_t), intent(in) :: size
        
        integer(8) :: j
        integer(c_int32_t) :: res
        integer(c_int8_t) :: pattern_elem1, pattern_elem2, pattern_elem3, pattern_elem4
        integer(c_int8_t) :: text_elem1, text_elem2, text_elem3, text_elem4
        
        pattern_elem1 = data(i)
        pattern_elem2 = data(i+1)
        pattern_elem3 = data(i+len-2)
        pattern_elem4 = data(i+len-1)
        
        res = 0_c_int32_t
        
        do j = 1, int(size, kind=8) - len + 1
            text_elem1 = data(j)
            text_elem2 = data(j+1)
            text_elem3 = data(j+len-2)
            text_elem4 = data(j+len-1)
            
            if ((pattern_elem1 == text_elem1) .and. &
                (pattern_elem2 == text_elem2) .and. &
                (pattern_elem3 == text_elem3) .and. &
                (pattern_elem4 == text_elem4)) then
                res = res + int(compare_strings_scalar(data, i, j, len), kind=c_int32_t)
            end if
        end do
        
        freq(i) = res
        
    end subroutine kernel

    include 'kernel_launcher.inc'

end program rabin_karp_SWAR_scalar_fortran
