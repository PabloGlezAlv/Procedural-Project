#include "pch.h"
#include "CircularMaze.h"
#include "Input.h"

CircularMaze::~CircularMaze()
{
}

CircularMaze::CircularMaze(int size, int nCircles, ID3D11Device* device, Light* light, DirectX::SimpleMath::Matrix* world, DirectX::SimpleMath::Matrix* view,
    DirectX::SimpleMath::Matrix* projection, Camera* cam)
{
    srand(time(NULL));

    mazeSize = size;
    circles = nCircles;

    //Set angles of rotation
    angles.resize(nCircles / 2);
    for (int i = 0; i < nCircles /2; ++i)
        angles[i] = 0;

    //Set speed of rotation
    speed.resize(nCircles / 2);
    for (int i = 0; i < nCircles / 2; i++)
        speed[i] =( i % 3 ) + 1; // Limit the speed 

    //Initialize
    maze.resize(nCircles);
    for (int i = 0; i < nCircles; ++i)
        maze[i].resize(mazeSize);
    
    //Fill the circles
    for (int i = 0; i < nCircles; ++i) {
        float radius = 50.0f + i * CIRCLES_DISTANCE;

        float angleIncrement = 2 * M_PI / mazeSize;

        //Calculate positions
        for (int j = 0; j < mazeSize; ++j) {
            float angle = j * angleIncrement;
            maze[i][j].positionX = radius * cos(angle);
            maze[i][j].positionZ = radius * sin(angle);

            maze[i][j].rotationY = 180 - (360/ mazeSize *j);

        }
    }

    //Set the correct path
    GenerateDistribution(0,indexStart);

    //Generate the gameObjects
    CreateObjects(device, light, world, view, projection, cam);
}

void CircularMaze::CreateObjects(ID3D11Device* device, Light* light, DirectX::SimpleMath::Matrix* world, DirectX::SimpleMath::Matrix* view,
    DirectX::SimpleMath::Matrix* projection, Camera* cam)
{
    for (int i = 0; i < circles; ++i) {
        for (int j = 0; j < mazeSize; ++j) {

            DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3(21 + (i * 2), 1, 1);
            DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(maze[i][j].positionX, 1, maze[i][j].positionZ);
            DirectX::XMFLOAT3 angle = DirectX::XMFLOAT3(0, 90 + maze[i][j].rotationY, 0);
            StaticEntity wall;
            //All posibilities
            if (maze[i][j].right && maze[i][j].left && maze[i][j].outside && maze[i][j].inside)
            {
                wall = StaticEntity(device, "mazeFloor", "lightNormal", light, world, view, projection, cam);
            }
            // 1 case miss
            else if (!maze[i][j].right && maze[i][j].left && maze[i][j].outside && maze[i][j].inside)
            {
                wall = StaticEntity(device, "mazeFloorRight", "lightNormal", light, world, view, projection, cam);
            }
            else if (maze[i][j].right && !maze[i][j].left && maze[i][j].outside && maze[i][j].inside)
            {
                wall = StaticEntity(device, "mazeFloorLeft", "lightNormal", light, world, view, projection, cam);
            }
            else if (maze[i][j].right && maze[i][j].left && !maze[i][j].outside && maze[i][j].inside)
            {
                wall = StaticEntity(device, "mazeFloorTop", "lightNormal", light, world, view, projection, cam);
            }
            else if (maze[i][j].right && maze[i][j].left && maze[i][j].outside && !maze[i][j].inside)
            {
                wall = StaticEntity(device, "mazeFloorDown", "lightNormal", light, world, view, projection, cam);
            }
            // 2 cases miss
            else if (!maze[i][j].right && !maze[i][j].left && maze[i][j].outside && maze[i][j].inside)
            {
                wall = StaticEntity(device, "mazeFloorRightLeft", "lightNormal", light, world, view, projection, cam);
            }
            else if (!maze[i][j].right && maze[i][j].left && maze[i][j].outside && !maze[i][j].inside)
            {
                wall = StaticEntity(device, "mazeFloorRightDown", "lightNormal", light, world, view, projection, cam);
            }
            else if (!maze[i][j].right && maze[i][j].left && !maze[i][j].outside && maze[i][j].inside)
            {
                wall = StaticEntity(device, "mazeFloorRightTop", "lightNormal", light, world, view, projection, cam);
            }
            else if (maze[i][j].right && !maze[i][j].left && maze[i][j].outside && !maze[i][j].inside)
            {
                wall = StaticEntity(device, "mazeFloorLeftDown", "lightNormal", light, world, view, projection, cam);
            }
            else if (maze[i][j].right && !maze[i][j].left && !maze[i][j].outside && maze[i][j].inside)
            {
                wall = StaticEntity(device, "mazeFloorLeftTop", "lightNormal", light, world, view, projection, cam);
            }
            else if (maze[i][j].right && maze[i][j].left && !maze[i][j].outside && !maze[i][j].inside)
            {
                wall = StaticEntity(device, "mazeFloorTopDown", "lightNormal", light, world, view, projection, cam);
            }
            // 3 cases miss
            else if (!maze[i][j].right && !maze[i][j].left && !maze[i][j].outside && maze[i][j].inside)
            {
                wall = StaticEntity(device, "mazeFloorRightLeftTop", "lightNormal", light, world, view, projection, cam);
            }
            else if (!maze[i][j].right && !maze[i][j].left && maze[i][j].outside && !maze[i][j].inside)
            {
                wall = StaticEntity(device, "mazeFloorRightLeftDown", "lightNormal", light, world, view, projection, cam);
            }
            else if (!maze[i][j].right && maze[i][j].left && !maze[i][j].outside && !maze[i][j].inside)
            {
                wall = StaticEntity(device, "mazeFloorRightTopDown", "lightNormal", light, world, view, projection, cam);
            }
            else if (maze[i][j].right && !maze[i][j].left && !maze[i][j].outside && !maze[i][j].inside)
            {
                wall = StaticEntity(device, "mazeFloorLeftTopDown", "lightNormal", light, world, view, projection, cam);
            }
            //wall = StaticEntity(device, "mazeFloor1", "lightNormal", light, world, view, projection, cam);

            wall.Initialize(position, scale, angle);
            mazeBlocks.push_back(wall);

            //Draw corridors between circles
            if (maze[i][j].outside)
            {
                float pointX = (maze[i][j].positionX + maze[i + 1][j].positionX) / 2;
                float pointZ = (maze[i][j].positionZ + maze[i + 1][j].positionZ) / 2;

                StaticEntity corridor = StaticEntity(device, "mazeFloorTopDown", "lightNormal", light, world, view, projection, cam);
                corridor.Initialize(DirectX::XMFLOAT3(pointX, 1, pointZ), DirectX::XMFLOAT3(2, 1, 1),
                    DirectX::XMFLOAT3(0, maze[i][j].rotationY, 0));
                mazeCorridors.push_back(corridor);
            }
        }
    }

    player = StaticEntity(device, "player", "lightNormal", light, world, view, projection, cam);
    player.Initialize(DirectX::XMFLOAT3(maze[0][0].positionX, 1.25f, maze[0][0].positionZ), DirectX::XMFLOAT3(0.25f, 0.25f, 0.25f), 
        DirectX::XMFLOAT3(0, 90, 0));

    buttonCircle = circles - 1;
    buttonRotation = rand() % mazeSize;

    button = StaticEntity(device, "box", "lightNormal", light, world, view, projection, cam);
    button.Initialize(DirectX::XMFLOAT3(maze[buttonCircle][buttonRotation].positionX, 0.65f, maze[buttonCircle][buttonRotation].positionZ),
        DirectX::XMFLOAT3(0.4f, 0.4f, 0.4f), DirectX::XMFLOAT3(0, 90 + maze[buttonCircle][buttonRotation].rotationY, 0));

    //Objecto to avois to limit the island
    limit = StaticEntity(device, "limit", "lightNormal", light, world, view, projection, cam);
    limit.Initialize(DirectX::XMFLOAT3(0, 0.95f, 0), DirectX::XMFLOAT3(12.09f,12.09f,12.09f), DirectX::XMFLOAT3(0, 0, 0));
}

void CircularMaze::HandleEvent(InputCommands* input)
{
    if (input->rightArrow && maze[playerCircle][playerRotation].right)
    {
        playerDir = right;
        startRotation = playerCenterRotation;
        playerMoving = true;

        player.setRotation(DirectX::XMFLOAT3(0, 0, 0));
    }
    else if (input->leftArrow && maze[playerCircle][playerRotation].left)
    {
        playerDir = left;
        startRotation = playerCenterRotation;
        playerMoving = true;

        player.setRotation(DirectX::XMFLOAT3(0, 180, 0));
    }
    else if (input->upArrow && maze[playerCircle][playerRotation].outside)
    {
        playerDir = top;
        startX = player.getPosition().x;
        playerMoving = true;

        player.setRotation(DirectX::XMFLOAT3(0, 90, 0));
    }
    else if (input->downArrow && maze[playerCircle][playerRotation].inside)
    {
        playerDir = down;
        startX = player.getPosition().x;
        playerMoving = true;

        player.setRotation(DirectX::XMFLOAT3(0, 270, 0));
    }
    else if (input->H)
    {
        ChangeButtonPosition();
    }
}


void CircularMaze::ReDoConections()
{
    for (int i = 0; i < circles; i++)
    {
        if (i % 2 == 0)
        {
            std::vector<roomInfo> temp(mazeSize);

            int realSpeed = speed[i / 2];

            //AnticlockWise
            if (i % 4 == 0) realSpeed = mazeSize - realSpeed;

            for (int j = 0; j < realSpeed; ++j) {
                temp[j] = maze[i][mazeSize - realSpeed + j];
            }

            for (int j = mazeSize - realSpeed - 1; j >= 0; --j) {
                maze[i][j + realSpeed] = maze[i][j];
            }

            for (int j = 0; j < realSpeed; ++j) {
                maze[i][j] = temp[j];
            }

            if(playerCircle == i)
            {
                playerRotation += realSpeed;
                if (playerRotation >= mazeSize) playerRotation -= mazeSize;
            }
        }
        else
        {
            for (int j = 0; j < mazeSize; j++)
            {
                maze[i - 1][j].outside = maze[i][j].inside;
                
                if (i > 1)
                {
                    maze[i - 1][j].inside = maze[i - 2][j].outside;
                }
            }
        }
    }
}

void CircularMaze::ChangeButtonPosition()
{
    points++;
    do //Make sure we dont repeat position
    {
        buttonRotation = rand() % mazeSize;
        buttonCircle = rand() % circles;
    } while (playerCircle == buttonCircle && playerRotation == buttonRotation);

    DirectX::SimpleMath::Vector3 position = button.getPosition();

    position.x = maze[buttonCircle][buttonRotation].positionX;
    position.z = maze[buttonCircle][buttonRotation].positionZ;
    button.setPosition(position);

    button.setRotation(DirectX::XMFLOAT3(0, 90 + maze[buttonCircle][buttonRotation].rotationY, 0));
}

void CircularMaze::Update(float deltaTime, InputCommands* input)
{
    //Rotate maze
    if (!rotating)
    {
        timer = timer + deltaTime;

        if (timer >= STOPTIME)
        {
            rotating = true;
            startAngles = angles;
        }

        if (!playerMoving) HandleEvent(input);
    }
    else if(!playerMoving)
    {
        int direction = 1;
        for (int i = 0; i < mazeBlocks.size(); i++)
        {
            int currentCircle = i / mazeSize;
            if (currentCircle % 2 == 0)
            {
                int index = currentCircle / 2;
                angles[index] = angles[index] + ANGLES_SECOND * speed[index] * deltaTime * direction;

                mazeBlocks[i].setParentRotation(DirectX::XMFLOAT3(0, angles[index], 0));

                if (playerCircle == currentCircle)
                {
                    playerCenterRotation = playerCenterRotation + ANGLES_SECOND * speed[index] * deltaTime * direction;
                    player.setParentRotation(DirectX::XMFLOAT3(0, playerCenterRotation, 0));
                }

                if ((i + 1) % mazeSize == 0)
                {
                    direction *= -1;
                }
            }
        }

        if (angles[0] >= startAngles[0] + (360 / mazeSize) * speed[0])
        {
            rotating = false;
            timer = 0;

            for (int i = 0; i < circles / 2; ++i)
            {
                if (i % 2 == 0)
                    angles[i] = startAngles[i] + (360 / mazeSize) * speed[0];
                else
                    angles[i] = -(startAngles[i] + (360 / mazeSize) * speed[0]);
            }

            ReDoConections();

            if (playerCircle == buttonCircle && playerRotation == buttonRotation)
            {
                ChangeButtonPosition();
            }
        }
    }


    //MovePlayer
    if (playerMoving)
    {

        switch (playerDir)
        {
            case right:
                playerCenterRotation = playerCenterRotation - PLAYER_ANGLES_SECOND * deltaTime;

                if (playerCenterRotation <= startRotation - (360 / mazeSize))
                {
                    playerCenterRotation = startRotation - (360 / mazeSize);

                    playerMoving = false;
                    playerRotation++;
                    if (playerRotation >= mazeSize) playerRotation = 0;
                }

                player.setParentRotation(DirectX::XMFLOAT3(0, playerCenterRotation, 0));
                break;
            case left:
                playerCenterRotation = playerCenterRotation + PLAYER_ANGLES_SECOND * deltaTime;

                if (playerCenterRotation >= startRotation + (360 / mazeSize) )
                {
                    playerCenterRotation = startRotation + (360 / mazeSize);

                    playerMoving = false;
                    playerRotation--;
                    if (playerRotation < 0) playerRotation = mazeSize - 1;
                }

                player.setParentRotation(DirectX::XMFLOAT3(0, playerCenterRotation, 0));
                break;
            case top:
                DirectX::XMFLOAT3 position = player.getPosition();
                position.x = position.x + 3 * deltaTime;

                if (position.x >= startX + CIRCLES_DISTANCE)
                {
                    position.x = startX + CIRCLES_DISTANCE;

                    playerMoving = false;
                    playerCircle++;
                }

                player.setPosition(position);
                break;
            case down:
                DirectX::XMFLOAT3 position2 = player.getPosition();
                position2.x = position2.x - 3 * deltaTime;

                if (position2.x <= startX - CIRCLES_DISTANCE)
                {
                    position2.x = startX - CIRCLES_DISTANCE;

                    playerMoving = false;
                    playerCircle--;
                }

                player.setPosition(position2);
                break;
        }
    }
}

void CircularMaze::Render(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture)
{
    for (int i = 0; i < mazeBlocks.size(); i++)
    {
        mazeBlocks[i].setShadowMap(texture);
        mazeBlocks[i].Render(deviceContext);
    }

    for (int i = 0; i < mazeCorridors.size(); i++)
    {
        mazeCorridors[i].setShadowMap(texture);
        mazeCorridors[i].Render(deviceContext);
    }

    player.setShadowMap(texture);
    player.Render(deviceContext);


    button.setShadowMap(texture);
    button.Render(deviceContext);

    limit.setShadowMap(texture);
    limit.Render(deviceContext);
}

void CircularMaze::RenderShadow(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* positionshadowView)
{
    for (int i = 0; i < mazeBlocks.size(); i++)
    {
        mazeBlocks[i].RenderShadow(context, positionshadowView);
    }
    for (int i = 0; i < mazeCorridors.size(); i++)
    {
        mazeCorridors[i].RenderShadow(context, positionshadowView);
    }

    player.RenderShadow(context, positionshadowView);

    button.RenderShadow(context, positionshadowView);

    limit.RenderShadow(context, positionshadowView);
}

int CircularMaze::getPoints()
{
    return points;
}

bool CircularMaze::isValid(int x, int y) {
    return x >= 0 && x < circles && y >= 0 && y < mazeSize;
}

void CircularMaze::GenerateDistribution(int i, int j)
{
    std::vector<std::pair<int, int>> directions = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

    std::random_shuffle(directions.begin(), directions.end());

    maze[i][j].visited++;

    for (const auto& dir : directions) {
        int newI = i + dir.first;
        int newJ = j + dir.second;
        
        if (isValid(newI, newJ) && maze[newI][newJ].visited == 0) {
            if (newI < i) {
                maze[i][j].inside = true;
                maze[newI][newJ].outside = true;
            }
            else if (newI > i) {
                maze[i][j].outside = true;
                maze[newI][newJ].inside = true;
            }
            else if (newJ < j) {
                maze[i][j].left = true;
                maze[newI][newJ].right = true;
            }
            else if (newJ > j) {
                maze[i][j].right = true;
                maze[newI][newJ].left = true;
            }

            GenerateDistribution(newI, newJ);
        }
    }
}
