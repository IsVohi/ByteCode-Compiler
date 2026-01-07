#!/bin/bash
# Azure VM Deployment Script

set -e

echo "====================================="
echo "ByteCode Compiler Azure Deployment"
echo "====================================="

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Configuration
REPO_URL="https://github.com/IsVohi/ByteCode-Compiler.git"
INSTALL_DIR="$HOME/ByteCode-Compiler"

echo -e "${GREEN}Step 1: Updating system...${NC}"
sudo apt update && sudo apt upgrade -y

echo -e "${GREEN}Step 2: Installing dependencies...${NC}"
sudo apt install -y build-essential cmake git

# Install Node.js
if ! command -v node &> /dev/null; then
    echo -e "${GREEN}Installing Node.js...${NC}"
    curl -fsSL https://deb.nodesource.com/setup_20.x | sudo -E bash -
    sudo apt install -y nodejs
fi

# Install NGINX
if ! command -v nginx &> /dev/null; then
    echo -e "${GREEN}Installing NGINX...${NC}"
    sudo apt install -y nginx
fi

echo -e "${GREEN}Step 3: Cloning repository...${NC}"
if [ -d "$INSTALL_DIR" ]; then
    echo "Directory exists, pulling latest changes..."
    cd "$INSTALL_DIR"
    git pull origin main
else
    git clone "$REPO_URL" "$INSTALL_DIR"
    cd "$INSTALL_DIR"
fi

echo -e "${GREEN}Step 4: Building compiler...${NC}"
mkdir -p build
cd build
cmake ..
make -j$(nproc)

if [ ! -f "./compiler" ]; then
    echo -e "${RED}Error: Compiler build failed${NC}"
    exit 1
fi

echo -e "${GREEN}Step 5: Setting up API server...${NC}"
cd "$INSTALL_DIR/server"
npm install

echo -e "${GREEN}Step 6: Creating systemd service...${NC}"
sudo tee /etc/systemd/system/compiler-api.service > /dev/null <<EOF
[Unit]
Description=ByteCode Compiler API
After=network.target

[Service]
Type=simple
User=$USER
WorkingDirectory=$INSTALL_DIR/server
ExecStart=/usr/bin/node api.js
Restart=on-failure
RestartSec=10
Environment=NODE_ENV=production

[Install]
WantedBy=multi-user.target
EOF

sudo systemctl daemon-reload
sudo systemctl enable compiler-api
sudo systemctl restart compiler-api

echo -e "${GREEN}Step 7: Configuring NGINX...${NC}"
sudo tee /etc/nginx/sites-available/compiler > /dev/null <<'EOF'
server {
    listen 80;
    server_name _;

    # API endpoints
    location /api {
        proxy_pass http://localhost:3000;
        proxy_http_version 1.1;
        proxy_set_header Upgrade \$http_upgrade;
        proxy_set_header Connection 'upgrade';
        proxy_set_header Host \$host;
        proxy_cache_bypass \$http_upgrade;
        proxy_set_header X-Real-IP \$remote_addr;
        proxy_set_header X-Forwarded-For \$proxy_add_x_forwarded_for;
        
        # CORS headers
        add_header Access-Control-Allow-Origin * always;
        add_header Access-Control-Allow-Methods 'GET, POST, OPTIONS' always;
        add_header Access-Control-Allow-Headers 'Content-Type' always;
    }
    
    # Health check
    location /health {
        proxy_pass http://localhost:3000/api/health;
    }
}
EOF

sudo ln -sf /etc/nginx/sites-available/compiler /etc/nginx/sites-enabled/
sudo nginx -t
sudo systemctl restart nginx

echo ""
echo -e "${GREEN}====================================="
echo "Deployment Complete!"
echo "=====================================${NC}"
echo ""
echo "API URL: http://$(curl -s ifconfig.me)/api"
echo ""
echo "Test with:"
echo "  curl http://$(curl -s ifconfig.me)/api/health"
echo ""
echo "View logs:"
echo "  sudo journalctl -u compiler-api -f"
echo ""
echo "Next steps:"
echo "  1. Configure your domain in NGINX"
echo "  2. Set up SSL with: sudo certbot --nginx"
echo "  3. Update frontend API_URL"
echo ""
