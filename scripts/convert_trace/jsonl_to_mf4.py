from asammdf import MDF, Signal
import sys
import numpy as np
import pandas as pd

input = sys.argv[1] # .jsonl
output = input + ".mdf" # .mf4
mdf = MDF(version="4.10")

# read jsonl file into pandas dataframe
dataframe = pd.read_json(input, lines=True)

# convert each can id to mdf signal
for can_id, group in dataframe.groupby("id"):
    # convert time to seconds from start
    times = pd.to_datetime(group["time"], format="%H:%M:%S.%f")
    t0 = times.iloc[0]
    timestamps = (times - t0).dt.total_seconds().to_numpy(dtype=np.float64)

    # bytes is tuple, make 2D uint8 array [n samples, n bytes]
    samples = np.array(group["bytes"].tolist(), dtype=np.uint8)

    # generate signal & append mdf file
    sig = Signal(
        name = str(can_id),
        samples = samples,
        timestamps = timestamps,
    )
    mdf.append(sig)

# save mdf file (.mf4)
mdf.save(output)
mdf.close()
