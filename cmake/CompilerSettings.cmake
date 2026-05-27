include_guard(GLOBAL)

function(create_project_options)
    # 1. User-facing option to toggle sanitizers
    option(ENABLE_SANITIZERS "Enable Sanitizers in Debug mode" OFF)
    # Create a toggle to enable/disable profiling
    option(TRACY_ENABLE "Enable Tracy profiling" OFF)
    option(TRACY_BUILD_PROFILER "Build Tracy GUI Profiler App automatically" OFF)

    # 2. Create the interface target if it doesn't exist
    if(NOT TARGET project_options)
        add_library(project_options INTERFACE)
    endif()

    # 3. Create readable aliases for complex logic
    set(IS_MSVC  $<CXX_COMPILER_ID:MSVC>)
    set(IS_GCC   $<CXX_COMPILER_ID:GNU>)
    set(IS_CLANG $<CXX_COMPILER_ID:Clang>)
    set(IS_DEBUG $<CONFIG:Debug>)
    
    # Logic for "Not MSVC" (Linux/Unix defaults)
    set(IS_POSIX $<OR:${IS_GCC},${IS_CLANG}>)

    # ---------------------------------------------------------
    # WARNING FLAGS
    # ---------------------------------------------------------
    if(TRACY_ENABLE)
        target_compile_options(project_options INTERFACE
            # Windows Warnings: Level 4 and treat as errors
            $<$<AND:${IS_MSVC}>:/W4>
            
            # Linux Warnings: Wall, Wextra, Pedantic, and treat as errors
            $<$<AND:${IS_POSIX}>:-Wall -Wextra -Wpedantic>
        )
    else()
        target_compile_options(project_options INTERFACE
            # Windows Warnings: Level 4 and treat as errors
            $<$<AND:${IS_MSVC}>:/W4 /WX>
            
            # Linux Warnings: Wall, Wextra, Pedantic, and treat as errors
            $<$<AND:${IS_POSIX}>:-Wall -Wextra -Wpedantic -Werror>
        )
    endif()

    # ---------------------------------------------------------
    # SANITIZER FLAGS (Only if enabled and in Debug mode)
    # ---------------------------------------------------------
    if(ENABLE_SANITIZERS)
        
        # --- MSVC (Windows) Sanitizers ---
        target_compile_options(project_options INTERFACE
            $<$<AND:${IS_MSVC},${IS_DEBUG}>:/fsanitize=address>
        )
        target_link_options(project_options INTERFACE
            $<$<AND:${IS_MSVC},${IS_DEBUG}>:/fsanitize=address>
        )

        # --- GCC/Clang (Linux) Sanitizers ---
        target_compile_options(project_options INTERFACE
            $<$<AND:${IS_POSIX},${IS_DEBUG}>:-fsanitize=address,undefined -fno-omit-frame-pointer>
        )
        target_link_options(project_options INTERFACE
            $<$<AND:${IS_POSIX},${IS_DEBUG}>:-fsanitize=address,undefined>
        )
        
    endif()

endfunction()