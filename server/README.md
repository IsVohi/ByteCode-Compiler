# Backend API Server

REST API for the ByteCode Compiler.

## Quick Start on Azure VM

```bash
# SSH into your Azure VM
ssh azureuser@your-vm-ip

# Run automated deployment
bash <(curl -s https://raw.githubusercontent.com/IsVohi/ByteCode-Compiler/main/server/deploy.sh)
```

## Manual Setup

```bash
# Install dependencies
npm install

# Start server
npm start

# Development with auto-reload
npm run dev
```

## API Endpoints

### `GET /api/health`
Check if API is running

**Response:**
```json
{
  "status": "ok",
  "timestamp": "2024-01-07T12:00:00Z",
  "compiler": "available"
}
```

### `POST /api/compile`
Compile and execute code

**Request:**
```json
{
  "code": "let x = 10;\nprint(x * 2);"
}
```

**Response (Success):**
```json
{
  "success": true,
  "output": "20\n",
  "executionTime": 123
}
```

**Response (Error):**
```json
{
  "success": false,
  "error": "Parser error: Expected ';' at line 1"
}
```

### `GET /api/examples`
Get example code snippets

**Response:**
```json
{
  "examples": [
    {
      "name": "Hello World",
      "code": "print(\"Hello, World!\");"
    }
  ]
}
```

## Configuration

Environment variables:
- `PORT` - Server port (default: 3000)
- `NODE_ENV` - Environment (development/production)

## Security

- **Timeout**: 5 seconds max execution
- **Rate Limiting**: TODO - Add rate limiting
- **Input Validation**: Max 50KB code size
- **Resource Limits**: Limited via timeout and maxBuffer

## Monitoring

```bash
# View logs
sudo journalctl -u compiler-api -f

# Check status
sudo systemctl status compiler-api

# Restart
sudo systemctl restart compiler-api
```

## Testing

```bash
# Health check
curl http://localhost:3000/api/health

# Compile test
curl -X POST http://localhost:3000/api/compile \
  -H "Content-Type: application/json" \
  -d '{"code": "print(42);"}'
```
