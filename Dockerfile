# SchedulX - C Backend + Frontend Container for Docker / Render / Railway
FROM alpine:latest

# Install GCC, musl development headers, and make
RUN apk add --no-cache gcc musl-dev make

WORKDIR /app

# Copy all source files
COPY . .

# Build the C server
RUN cd backend && make clean && make

# SchedulX port
EXPOSE 8080

# Run the C server serving the frontend
CMD ["./backend/bin/schedulx", "8080", "frontend"]
