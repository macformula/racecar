Import("env")

flags = [
    "-mfloat-abi=hard",
    "-mfpu=fpv5-d16",
]
    
env.Append(CCFLAGS=flags, LINKFLAGS=flags)