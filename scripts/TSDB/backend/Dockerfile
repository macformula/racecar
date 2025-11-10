FROM python:3.11-slim

WORKDIR /app

# Copy auth requirements first since main requirements.txt references it
COPY auth/requirements.txt ./auth/requirements.txt
COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

COPY . /app

ENV PYTHONUNBUFFERED=1
EXPOSE 8000

CMD ["uvicorn", "server:app", "--host", "0.0.0.0", "--port", "8000", "--reload"]
