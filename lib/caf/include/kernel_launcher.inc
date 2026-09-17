subroutine run_fortran_kernel(data, freq, len, size, time, block_start, block_end)
    use iso_c_binding
    use iso_fortran_env
    implicit none
    
    integer(c_int8_t), intent(in) :: data(:)
    integer(c_int32_t), allocatable, intent(inout) :: freq(:)[:]
    integer(c_int32_t), value :: len
    integer(c_size_t), value :: size
    real(c_double), intent(out) :: time
    integer(8), value :: block_start, block_end
    
    integer(8) :: t1, t2, rate
    
    integer(8) :: i

    call system_clock(t1, rate)
      
    do i = block_start, block_end, 1
        call kernel(i, data, freq, len, size)
    end do
    
    sync all
    
    call system_clock(t2)
    
    time = real(t2 - t1, c_double) / real(rate, c_double)
    
end subroutine run_fortran_kernel


subroutine read_input_file(input_file, data, size)
    use iso_c_binding
    use iso_fortran_env
    implicit none
    
    character(len=*), intent(in) :: input_file
    integer(c_int8_t), allocatable, intent(out) :: data(:)
    integer(c_size_t), intent(out) :: size
    
    character(:), allocatable :: file_string
    integer(8) :: file_size, i
    integer :: ios
    
    inquire(file=trim(input_file), size=file_size)
    if (file_size == -1) stop 'Input file size error'
  
    allocate(character(file_size) :: file_string)
    open(10, file=trim(input_file), access='stream', form='unformatted', status='old', iostat=ios)
    if (ios /= 0) stop 'Input file: open error'
    read(10, iostat=ios) file_string
    if (ios /= 0) stop 'Input file: read error'
    close(10)
    
    file_size = len_trim(file_string)
    size = int(file_size, kind=c_size_t)
    
    allocate(data(file_size + 4097))

    do i = 1, file_size + 4097
        data(i) = 5
    end do  
    do i = 1, file_size
        select case(file_string(i:i))
        case('A'); data(i) = 0
        case('C'); data(i) = 1
        case('G'); data(i) = 2
        case('T'); data(i) = 3
        case default; data(i) = 5
        end select
    end do
      
    deallocate(file_string)
    
end subroutine read_input_file


subroutine write_result_file(output_file, freq, size, time, block_size)
    use iso_c_binding
    implicit none
    
    character(len=*), intent(in) :: output_file
    real(c_double), intent(in) :: time
    integer(c_int32_t), allocatable, intent(inout) :: freq(:)[:]
    integer(c_size_t), intent(in) :: size
    integer(8), value :: block_size
    
    integer(8) :: block_start, block_end, img
    
    integer :: ios, i
    
    if (this_image() == 1) then
        do img = 2, num_images()
            block_start = (img - 1)*block_size + 1
            block_end = min(img * block_size, int(size - len + 1, kind=8))
            
            freq(block_start:block_end) = freq(block_start:block_end)[img]
        end do
    
        open(20, file=trim(output_file), access='stream', form='unformatted', status='replace', iostat=ios)
        if (ios /= 0) stop 'Output file: open error'
        
        write(20, iostat=ios) time
        if (ios /= 0) stop 'Output file: time writing error'
        write(20, iostat=ios) size
        if (ios /= 0) stop 'Output file: size writing error'
        do i = 1, int(size)
            write(20, iostat=ios) freq(i)
            if (ios /= 0) stop 'Output file: element writing error'
        end do
        
        close(20)
    end if
    sync all
        
end subroutine write_result_file
