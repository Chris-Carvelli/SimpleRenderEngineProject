# Bunny Jump Platformer

## Setup
- Move te exercise folder folder inside SimpleRenderEngineProject
- Add exercise folder as subdirectory at the end of `SimpleRenderEngineProject/CMakeLists.txt`
```
# ...

### misc
mark_as_advanced(
	SDL2MAIN_LIBRARY
)


# add this here
add_subdirectory(PlatformerProject)
```

## Controls

| Key   | Action          |
|-------|-----------------|
| A/D   | move left/right |
| F1    | SRE debugger    |
| F2    | b2d debugger    |


