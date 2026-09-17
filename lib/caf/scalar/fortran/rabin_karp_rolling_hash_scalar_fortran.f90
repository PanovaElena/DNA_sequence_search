program rabin_karp_rolling_hash_scalar_fortran
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
    
    if (command_argument_count() < 3) then
        stop 'Usage: program <input_file> <len> <output_file>'
    end if
    
    call get_command_argument(1, input_file)
    call get_command_argument(2, len_str)
    call get_command_argument(3, output_file)
    read(len_str, *, iostat=ios) len
    if (ios /= 0) stop 'Invalid length argument'
    
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
    
    call write_result_file(output_file, freq, size, time, block_size)
    
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
        
        integer(8) :: j, k
        integer(c_int32_t) :: res
        integer(8) :: hash_pattern, hash_text, p, powmod
        
        res = 0_c_int32_t
        
        hash_pattern = 0
        hash_text = 0
        p = 2
        powmod = 1
        do j = 1, len
            hash_pattern = hash_pattern * p + int(data(i + j - 1), 8)
            hash_text = hash_text * p + int(data(j), 8)
            powmod = powmod * p
        end do
        do j = 1, size - len + 1
            if (hash_text == hash_pattern) then
                res = res + int(compare_strings_scalar(data, i, j, len), kind=c_int32_t)
            end if
            hash_text = hash_text * p - int(data(j), 8) * powmod + int(data(j + len), 8)
        end do
        
        freq(i) = res
        
    end subroutine kernel

    include 'kernel_launcher.inc'

end program rabin_karp_rolling_hash_scalar_fortran
