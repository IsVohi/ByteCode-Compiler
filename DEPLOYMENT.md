# Azure Deployment Guide

This guide shows how to deploy the ByteCode Compiler on Azure VM with a REST API backend.

## Architecture

```
┌──────────────────┐         ┌──────────────────────┐
│   Frontend       │         │   Azure VM           │
│   (Vercel)       │ ─HTTP──>│   ┌────────────┐     │
│   - Code Editor  │         │   │ Node.js    │     │
│   - Submit Code  │         │   │ Express    │     │
│   - Show Output  │         │   │ API        │     │
└──────────────────┘         │   └─────┬──────┘     │
                             │         │            │
                             │   ┌─────▼──────┐     │
                             │   │ Compiler   │     │
                             │   │ Binary     │     │
                             │   └────────────┘     │
                             └──────────────────────┘
```

## Step 1: Set Up Azure VM

### Create VM
1. Go to Azure Portal
2. Create a new Virtual Machine:
   - **OS**: Ubuntu 22.04 LTS
   - **Size**: Standard B2s (2 vCPUs, 4 GB RAM) or higher
   - **Authentication**: SSH public key
3. Add inbound port rules:
   - Port 22 (SSH)
   - Port 80 (HTTP)
   - Port 443 (HTTPS)
   - Port 3000 (API - temporary, will use NGINX)

### Connect to VM
```bash
ssh azureuser@<your-vm-ip>
```

## Step 2: Install Dependencies on Azure VM

```bash
# Update system
sudo apt update && sudo apt upgrade -y

# Install build tools
sudo apt install -y build-essential cmake git

# Install Node.js
curl -fsSL https://deb.nodesource.com/setup_20.x | sudo -E bash -
sudo apt install -y nodejs

# Install NGINX
sudo apt install -y nginx

# Install certbot for SSL
sudo apt install -y certbot python3-certbot-nginx
```

## Step 3: Deploy Compiler on Azure VM

### Upload and build compiler
```bash
# Clone your repository
cd ~
git clone https://github.com/IsVohi/ByteCode-Compiler.git
cd ByteCode-Compiler

# Build the compiler
mkdir build && cd build
cmake ..
make

# Verify it works
./compiler
```

## Step 4: Set Up Backend API

### Install dependencies
```bash
cd ~/ByteCode-Compiler
npm init -y
npm install express cors body-parser
```

### Create API server (see `server/api.js`)

The backend API is in the `server/` folder. It provides:
- `POST /api/compile` - Compile and run code
- `POST /api/repl` - Interactive REPL session
- `GET /api/health` - Health check

## Step 5: Run Backend as a Service

### Create systemd service
```bash
sudo nano /etc/systemd/system/compiler-api.service
```

Add:
```ini
[Unit]
Description=ByteCode Compiler API
After=network.target

[Service]
Type=simple
User=azureuser
WorkingDirectory=/home/azureuser/ByteCode-Compiler/server
ExecStart=/usr/bin/node api.js
Restart=on-failure
RestartSec=10

[Install]
WantedBy=multi-user.target
```

Enable and start:
```bash
sudo systemctl daemon-reload
sudo systemctl enable compiler-api
sudo systemctl start compiler-api
sudo systemctl status compiler-api
```

## Step 6: Configure NGINX

```bash
sudo nano /etc/nginx/sites-available/compiler
```

Add:
```nginx
server {
    listen 80;
    server_name your-domain.com;  # Replace with your domain or IP

    location /api {
        proxy_pass http://localhost:3000;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection 'upgrade';
        proxy_set_header Host $host;
        proxy_cache_bypass $http_upgrade;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    }
}
```

Enable site:
```bash
sudo ln -s /etc/nginx/sites-available/compiler /etc/nginx/sites-enabled/
sudo nginx -t
sudo systemctl restart nginx
```

## Step 7: Add SSL (Optional but Recommended)

```bash
sudo certbot --nginx -d your-domain.com
```

## Step 8: Test the API

```bash
# Test health endpoint
curl http://your-vm-ip/api/health

# Test compilation
curl -X POST http://your-vm-ip/api/compile \
  -H "Content-Type: application/json" \
  -d '{"code": "let x = 10; print(x * 2);"}'
```

## Frontend Integration

From your Vercel frontend, call the API:

```javascript
const API_URL = 'https://your-domain.com/api';

async function compileCode(code) {
    const response = await fetch(`${API_URL}/compile`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ code })
    });
    return await response.json();
}
```

## Security Considerations

1. **Rate Limiting**: Add rate limiting to prevent abuse
2. **Timeout**: Set execution timeouts (default: 5 seconds)
3. **Resource Limits**: Limit memory and CPU usage
4. **Input Validation**: Sanitize input code
5. **Firewall**: Configure Azure NSG properly
6. **SSL**: Always use HTTPS in production

## Monitoring

```bash
# View API logs
sudo journalctl -u compiler-api -f

# View NGINX logs
sudo tail -f /var/log/nginx/access.log
sudo tail -f /var/log/nginx/error.log

# Monitor system resources
htop
```

## Updating the Compiler

```bash
cd ~/ByteCode-Compiler
git pull origin main
cd build
cmake ..
make
sudo systemctl restart compiler-api
```

## Troubleshooting

### API not responding
```bash
sudo systemctl status compiler-api
sudo journalctl -u compiler-api -n 50
```

### Build errors
```bash
cd ~/ByteCode-Compiler/build
cmake ..
make clean && make
```

### Port already in use
```bash
sudo lsof -i :3000
sudo kill -9 <PID>
```

## Cost Estimation

- **Azure VM B2s**: ~$30-40/month
- **Domain**: ~$10-15/year
- **SSL**: Free (Let's Encrypt)

**Total**: ~$30-40/month

---

**Next Steps:**
1. Follow this guide to deploy on Azure
2. Create frontend (see `frontend/` folder)
3. Deploy frontend on Vercel
4. Connect frontend to Azure backend
